"""Strict, dependency-free UPS/BPS application; no ROM acquisition or I/O.

Usage::

    info = inspect_patch(patch_bytes)
    output = apply_patch(source_bytes, patch_bytes, ups_direction="forward")

UPS is reversible. Use ups_direction="reverse" when the documented required
input matches the target endpoint in the UPS header; "auto" selects by BOTH
size and CRC32. BPS supports forward application only. There is no force mode.

Format behavior cross-checked against the primary Floating IPS implementation:
https://github.com/Alcaro/Flips/blob/master/libups.cpp
https://github.com/Alcaro/Flips/blob/master/libbps.cpp

This is an independent Python implementation of the binary formats. Inputs are
never changed. Nothing is returned unless patch, source and result checks pass.
"""

from dataclasses import dataclass
from typing import Literal
import struct
import zlib

__all__ = ["PatchError", "PatchInfo", "inspect_patch", "apply_patch",
           "apply_ups", "apply_bps"]

DEFAULT_MAX_SIZE = 64 * 1024 * 1024
MAX_INTEGER = (1 << 63) - 1
Direction = Literal["auto", "forward", "reverse"]


class PatchError(ValueError):
    """Invalid patch, wrong source, exceeded bound, or wrong result."""


@dataclass(frozen=True)
class PatchInfo:
    format: str
    source_size: int
    target_size: int
    source_crc32: int
    target_crc32: int
    patch_crc32: int
    metadata: bytes = b""


def _crc(data: bytes) -> int:
    return zlib.crc32(data) & 0xFFFFFFFF


class _Reader:
    def __init__(self, data: bytes, end: int):
        self.data = data
        self.end = end
        self.pos = 4

    def byte(self) -> int:
        if self.pos >= self.end:
            raise PatchError("Truncated patch instruction or variable-length integer")
        value = self.data[self.pos]
        self.pos += 1
        return value

    def take(self, count: int) -> bytes:
        if count < 0 or count > self.end - self.pos:
            raise PatchError("Patch literal or metadata crosses the checksum footer")
        value = self.data[self.pos:self.pos + count]
        self.pos += count
        return value

    def number(self) -> int:
        # UPS/BPS biased base-128, distinct from ordinary LEB128.
        value, weight = 0, 1
        for _ in range(10):
            byte = self.byte()
            value += (byte & 0x7F) * weight
            if value > MAX_INTEGER:
                raise PatchError("Variable-length integer exceeds 63 bits")
            if byte & 0x80:
                return value
            weight <<= 7
            value += weight
        raise PatchError("Overlong variable-length integer")

    def signed(self) -> int:
        encoded = self.number()
        return -(encoded >> 1) if encoded & 1 else encoded >> 1


def _header(patch: bytes, max_size: int) -> tuple[PatchInfo, _Reader]:
    if not isinstance(max_size, int) or max_size < 0:
        raise ValueError("max_size must be a non-negative integer")
    signature = patch[:4]
    if signature not in (b"UPS1", b"BPS1"):
        raise PatchError("Unsupported patch signature; expected UPS1 or BPS1")
    minimum = 18 if signature == b"UPS1" else 19
    if len(patch) < minimum:
        raise PatchError("Patch is too short to contain its header and CRC32 footer")
    source_crc, target_crc, patch_crc = struct.unpack("<III", patch[-12:])
    actual_patch_crc = _crc(patch[:-4])
    if actual_patch_crc != patch_crc:
        raise PatchError(f"Patch CRC32 mismatch: {actual_patch_crc:08X} != {patch_crc:08X}")
    reader = _Reader(patch, len(patch) - 12)
    source_size, target_size = reader.number(), reader.number()
    if max(source_size, target_size) > max_size:
        raise PatchError(f"Declared file size exceeds max_size={max_size}")
    metadata = reader.take(reader.number()) if signature == b"BPS1" else b""
    return PatchInfo(signature[:3].decode(), source_size, target_size,
                     source_crc, target_crc, patch_crc, metadata), reader


def inspect_patch(patch: bytes, *, max_size: int = DEFAULT_MAX_SIZE) -> PatchInfo:
    """Validate header/patch CRC and return endpoints; actions checked on apply."""
    return _header(bytes(patch), max_size)[0]


def _check_source(source: bytes, size: int, crc: int) -> None:
    actual_crc = _crc(source)
    if len(source) != size or actual_crc != crc:
        raise PatchError(
            f"Wrong source: size={len(source)}, CRC32={actual_crc:08X}; "
            f"required size={size}, CRC32={crc:08X}")


def _check_result(output: bytearray, size: int, crc: int) -> bytes:
    if len(output) != size:
        raise PatchError(f"Result size mismatch: {len(output)} != {size}")
    actual_crc = _crc(output)
    if actual_crc != crc:
        raise PatchError(f"Result CRC32 mismatch: {actual_crc:08X} != {crc:08X}")
    return bytes(output)


def apply_ups(source: bytes, patch: bytes, *, direction: Direction = "auto",
              max_size: int = DEFAULT_MAX_SIZE) -> bytes:
    """Apply an UPS patch, selecting direction using exact size AND CRC32."""
    source, patch = bytes(source), bytes(patch)
    info, reader = _header(patch, max_size)
    if info.format != "UPS":
        raise PatchError("apply_ups requires an UPS1 patch")
    if direction not in ("auto", "forward", "reverse"):
        raise ValueError("UPS direction must be auto, forward or reverse")
    endpoint = (len(source), _crc(source))
    forward_match = endpoint == (info.source_size, info.source_crc32)
    reverse_match = endpoint == (info.target_size, info.target_crc32)
    if direction == "auto":
        if forward_match:
            reverse = False
        elif reverse_match:
            reverse = True
        else:
            raise PatchError(
                f"Source matches neither UPS endpoint: size={endpoint[0]}, "
                f"CRC32={endpoint[1]:08X}; expected "
                f"{info.source_size}/{info.source_crc32:08X} or "
                f"{info.target_size}/{info.target_crc32:08X}")
    else:
        reverse = direction == "reverse"
        _check_source(source,
                      info.target_size if reverse else info.source_size,
                      info.target_crc32 if reverse else info.source_crc32)
    result_size = info.source_size if reverse else info.target_size
    result_crc = info.source_crc32 if reverse else info.target_crc32
    span = max(info.source_size, info.target_size)
    output = bytearray(span)
    output[:len(source)] = source
    offset = 0
    while reader.pos < reader.end:
        offset += reader.number()
        if offset > span:
            raise PatchError("UPS relative offset exceeds either file's extent")
        while True:
            delta = reader.byte()
            if delta == 0:
                # A zero terminator consumes a position; at EOF it may be
                # exactly one past the last real byte. It never changes data.
                offset += 1
                break
            if offset >= span:
                raise PatchError("UPS XOR data extends beyond either file")
            output[offset] ^= delta
            offset += 1
    del output[result_size:]
    return _check_result(output, result_size, result_crc)


def apply_bps(source: bytes, patch: bytes, *,
              max_size: int = DEFAULT_MAX_SIZE) -> bytes:
    """Apply all four BPS operations, including overlapping TargetCopy."""
    source, patch = bytes(source), bytes(patch)
    info, reader = _header(patch, max_size)
    if info.format != "BPS":
        raise PatchError("apply_bps requires a BPS1 patch")
    _check_source(source, info.source_size, info.source_crc32)
    output = bytearray()
    source_relative = target_relative = 0
    while reader.pos < reader.end:
        instruction = reader.number()
        action, count = instruction & 3, (instruction >> 2) + 1
        if count > info.target_size - len(output):
            raise PatchError("BPS instruction exceeds declared target size")
        if action == 0:  # SourceRead, aligned to current target position.
            start = len(output)
            if count > len(source) - start:
                raise PatchError("BPS SourceRead exceeds source size")
            output.extend(source[start:start + count])
        elif action == 1:  # TargetRead, literal bytes from the patch.
            output.extend(reader.take(count))
        elif action == 2:  # SourceCopy, signed movement of source cursor.
            source_relative += reader.signed()
            if source_relative < 0 or count > len(source) - source_relative:
                raise PatchError("BPS SourceCopy refers outside the source")
            output.extend(source[source_relative:source_relative + count])
            source_relative += count
        else:  # TargetCopy can read bytes that it has itself just appended.
            target_relative += reader.signed()
            if not 0 <= target_relative < len(output):
                raise PatchError("BPS TargetCopy must start in already-written data")
            distance = len(output) - target_relative
            if count <= distance:
                output.extend(output[target_relative:target_relative + count])
            else:
                period = bytes(output[target_relative:])
                repeats, tail = divmod(count, distance)
                output.extend(period * repeats)
                output.extend(period[:tail])
            target_relative += count
    return _check_result(output, info.target_size, info.target_crc32)


def apply_patch(source: bytes, patch: bytes, *,
                ups_direction: Direction = "auto",
                max_size: int = DEFAULT_MAX_SIZE) -> bytes:
    """Dispatch by signature. Return verified immutable bytes; perform no I/O."""
    patch = bytes(patch)
    if patch.startswith(b"UPS1"):
        return apply_ups(source, patch, direction=ups_direction, max_size=max_size)
    if patch.startswith(b"BPS1"):
        return apply_bps(source, patch, max_size=max_size)
    raise PatchError("Unsupported patch signature; expected UPS1 or BPS1")

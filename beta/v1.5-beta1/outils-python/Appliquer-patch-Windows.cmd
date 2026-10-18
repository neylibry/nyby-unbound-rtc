@echo off
setlocal
where py >nul 2>nul
if not errorlevel 1 (
  py -3 "%~dp0appliquer_patch.py" %*
) else (
  where python >nul 2>nul
  if errorlevel 1 (
    echo Python 3.10+ requis / Python 3.10+ required.
    echo Voir README_FR.md / See README_EN.md.
  ) else (
    python "%~dp0appliquer_patch.py" %*
  )
)
pause

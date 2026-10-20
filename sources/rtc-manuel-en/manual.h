/* Original NyBy manual UI. Reuse the startup font/rasterizer, no GUI library. */
static const char *const manual_labels[5]={
    MSG("Jour","Day"),MSG("Mois","Month"),MSG("Ann\xE9""e","Year"),
    MSG("Heure","Hour"),MSG("Minute","Minute")
};
static void manual_header(void) {
    panel_begin();
    banner_center(8,"RTC UNBOUND",2,2);
}
static void manual_footer(const char *top,const char *bottom) {
    banner_center(122,top,1,3);
    banner_center(136,bottom,1,4);
    banner_footer();
    panel_present();
}
static void manual_values(const Fields *f,u32 field) {
    static const u8 values_x[5]={61,97,133,91,127};
    static const u8 labels_x[5]={NYBY_EN?63:60,NYBY_EN?94:96,NYBY_EN?145:142,NYBY_EN?91:87,121};
    for(u32 i=0;i<5;i++) {
        char value[5];u32 width=i==2?4:2,y=i<3?59:96;
        banner_digits(value,field_value(f,i),width);value[width]=0;
        banner_text(labels_x[i],i<3?46:83,manual_labels[i],1,i==field?1:3);
        banner_text(values_x[i],y,value,2,i==field?4:1);
        if(i==field)for(u32 x=0;x<width*12-2;x++)banner_pixel(values_x[i]+x,y+17,4);
    }
    banner_text(85,59,"/",2,3);banner_text(121,59,"/",2,3);
    banner_text(115,96,":",2,3);

}
static void screen(const Fields *f,u32 field) {
    manual_header();
    banner_center(30,MSG("Horloge manuelle - NyBy/Codex","Manual clock - NyBy/Codex"),1,3);
    manual_values(f,field);
    manual_footer(MSG("< > Champ  Haut/Bas Valeur","< > Field  Up/Down Value"),
                  MSG("start - lancer le jeu","start - launch game"));
    panel_clone();
}
static void screen_update(const Fields *f,u32 field) {
    panel_clear(5,10);manual_values(f,field);panel_present();
}
static void screen_notice(u32 error) {
    manual_header();
    banner_center(76,error?MSG("R\xE9glage non enregistr\xE9","Setting not saved"):
                              MSG("Enregistrement...","Saving setting..."),1,error?4:1);
    manual_footer(error?MSG("A R\xE9""essayer","A Retry"):"",
                  error?MSG("B Continuer","B Continue"):"");
}


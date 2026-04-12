#pragma once
// ============================================================
// TimeFontMetrics.H
// Dimensions dérivées de la police, partagées par
// Fl_Time_Input et Fl_Time_Picker.
// ============================================================
#include <FL/fl_draw.H>
#include <FL/Enumerations.H>

struct TimeFontMetrics {
    Fl_Font font;    // police
    int     size;    // taille en points
    int     ch;      // hauteur de cellule  (fl_height + 2×pad)
    int     cw_num;  // largeur de "00"     + 2×pad
    int     cw_ampm; // largeur de "AM/PM"  + 2×pad
    int     sep_w;   // largeur de ":"      + pad
    int     pad;     // marge interne       (≈ fl_height/3)

    // Mesure toutes les dimensions avec la police (f, s).
    // Doit être appelée dans un contexte de rendu FLTK valide
    // (pendant draw() ou après fl_open_display()).
    void measure(Fl_Font f, int s) {
        font = f;
        size = s;
        fl_font(f, s);
        int fh = fl_height();
        pad     = fh / 3;
        ch      = fh + pad * 2;
        cw_num  = (int)fl_width("00")    + pad * 2;
        cw_ampm = (int)fl_width("AM/PM") + pad * 2;
        sep_w   = (int)fl_width(":")     + pad;
    }
};

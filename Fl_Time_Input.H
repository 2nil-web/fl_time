#pragma once
// ============================================================
// Fl_Time_Input.H
// Widget FLTK imitant le <input type="time"> de Chrome.
// Toutes les dimensions sont proportionnelles à la police
// assignée via labelfont() / labelsize().
//
// Dépendances : FLTK 1.3 ou 1.4
// Compilation :
//   g++ main.cpp Fl_Time_Picker.cpp Fl_Time_Input.cpp \
//       -o demo $(fltk-config --cxxflags --ldflags)
// ============================================================
#include <FL/Fl_Group.H>
#include "TimeFontMetrics.H"
#include "Fl_Time_Picker.H"

enum TimeSegment { SEG_HOUR = 0, SEG_MIN, SEG_SEC, SEG_AMPM, SEG_NONE };

class Fl_Time_Input : public Fl_Group {
public:
    Fl_Time_Input(int x, int y, int w, int h, const char* label = nullptr);
    ~Fl_Time_Input();

    // Valeur courante
    void set_time(int hour12, int min, int sec, bool pm);
    int  hour12() const { return h_; }
    int  minute() const { return m_; }
    int  second() const { return s_; }
    bool is_pm()  const { return pm_; }
    int  hour24() const {
        return pm_ ? (h_ == 12 ? 12 : h_ + 12)
                   : (h_ == 12 ?  0 : h_);
    }

    // Police : utilisez labelfont() / labelsize() hérités de Fl_Widget.
    // Le picker se synchronise automatiquement à chaque ouverture.

    // Retourne la taille exacte requise par la police courante.
    // Appelez après labelfont()/labelsize(), avant show().
    void preferred_size(int& pw, int& ph) const;

    // Redimensionne le widget à preferred_size() en conservant x() et y().
    void fit();

    // Surcharge resize() pour imposer preferred_size() :
    // accepte x/y librement, mais corrige w/h à la taille calculée.
    void resize(int x, int y, int w, int h) override;

    // Callback déclenché à chaque modification de la valeur.
    void callback(Fl_Callback* cb, void* data = nullptr);

private:
    int         h_, m_, s_;
    bool        pm_;
    TimeSegment active_seg_;

    Fl_Time_Picker* picker_;
    Fl_Callback*    user_cb_;
    void*           user_data_;

    TimeFontMetrics fm_;

    // Géométrie interne recalculée à chaque draw / handle.
    struct Rects {
        int fx, fy, fw, fh;        // champ texte
        int sx, sy, sw, sh;        // bouton spin
        int cx, cy, cw, ch;        // bouton horloge
        int seg_x[4], seg_w[4];    // segments HH MM SS AM/PM
    } r_;

    void measure_font();
    void compute_rects();

    void draw()         override;
    int  handle(int ev) override;

    void draw_segment(const char* txt, int x, int y, int w, int h, bool active);
    void draw_spin_button(int x, int y, int w, int h);
    void draw_clock_button(int x, int y, int w, int h);

    void activate_segment(TimeSegment s);
    void next_segment();
    void prev_segment();
    void increment(int delta);
    void open_picker();
    void fire_callback();
};

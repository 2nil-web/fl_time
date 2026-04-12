#ifndef FL_TIME_PICKER_H
#define FL_TIME_PICKER_H
// ============================================================
// Fl_Time_Picker.H
// Fenêtre popup à colonnes déroulantes (HH / MM / SS / AM-PM).
// - Validation  : clic sur OK ou touche Entrée
// - Annulation  : clic en dehors de la popup, touche Échap,
//                 ou double-clic sur la même valeur.
// Le bouton "Annuler" est supprimé ; la fermeture sans
// validation est gérée par Fl::grab().
// ============================================================
#include <FL/Fl_Window.H>
#include <functional>
#include "TimeFontMetrics.H"

class Fl_Time_Picker : public Fl_Window {
public:
    struct Time { int h, m, s; bool pm; };

    // Déclenché uniquement en cas de validation (OK / Entrée).
    std::function<void(Time)> on_accept;

    Fl_Time_Picker();

    void set_metrics(const TimeFontMetrics& m);
    void show_at(int screen_x, int screen_y, Time t);

private:
    static const int ROWS_V = 5;

    TimeFontMetrics fm_;
    Time            cur_;
    int             sc_[4];

    // Dimensions dérivées des métriques.
    int col_w_[4];
    int header_h_;
    int footer_h_;  // footer réduit : contient uniquement le bouton OK
    int btn_h_;
    int btn_w_;

    static int col_count(int col) { return col == 0 ? 12 : col == 3 ? 2 : 60; }

    void layout();

    int  sel_index(int col) const { return sc_[col]; }
    void clamp_scroll(int col);
    void update_cur_from_scroll();

    // Valide et ferme.
    void accept_and_hide();
    // Ferme sans valider (libère aussi le grab).
    void cancel_and_hide();

    void draw()         override;
    int  handle(int ev) override;
    void draw_col(int col, int cx);

    int clip_top()    const { return header_h_; }
    int clip_bottom() const { return header_h_ + ROWS_V * fm_.ch; }
    int col_x(int c)  const;
    int col_at(int mx) const;
    void item_label(int col, int idx, char* buf, int bufsz) const;

    // Géométrie du seul bouton OK (centré dans le footer).
    int ok_x() const { return (w() - btn_w_) / 2; }
    int btn_y() const { return h() - footer_h_ + (footer_h_ - btn_h_) / 2; }
};
#endif /* FL_TIME_PICKER_H */

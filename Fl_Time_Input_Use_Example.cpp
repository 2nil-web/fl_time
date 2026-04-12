// main.cpp — démonstration de Fl_Time_Input avec trois polices
//
// Compilation :
//   g++ main.cpp Fl_Time_Picker.cpp Fl_Time_Input.cpp \
//       -o time_demo $(fltk-config --cxxflags --ldflags)
//
#include "Fl_Time_Input.H"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>

static Fl_Box *info_box = nullptr;

static void time_cb(Fl_Widget *w, void *)
{
  auto *t = (Fl_Time_Input *)w;
  char buf[64];
  snprintf(buf, sizeof(buf), "Valeur : %02d:%02d:%02d %s  (24h : %02d:%02d:%02d)", t->hour12(), t->minute(), t->second(), t->is_pm() ? "PM" : "AM", t->hour24(), t->minute(), t->second());
  info_box->copy_label(buf);
}

int main()
{
  Fl_Window *win = new Fl_Window(520, 260, "Fl_Time_Input — proportionnel");
  win->begin();

  // Ligne 1 : Helvetica 14pt
  {
    Fl_Box *l = new Fl_Box(10, 14, 150, 22, "Normal (14pt) :");
    l->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    Fl_Time_Input *ti = new Fl_Time_Input(165, 10, 280, 26);
    ti->labelfont(FL_HELVETICA);
    ti->labelsize(14);
    ti->set_time(10, 30, 0, false);
    ti->callback(time_cb);
  }

  // Ligne 2 : Helvetica Bold 20pt
  {
    Fl_Box *l = new Fl_Box(10, 54, 150, 30, "Grand (20pt) :");
    l->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    Fl_Time_Input *ti = new Fl_Time_Input(165, 50, 330, 36);
    ti->labelfont(FL_HELVETICA_BOLD);
    ti->labelsize(20);
    ti->set_time(8, 0, 0, true);
    ti->callback(time_cb);
  }

  // Ligne 3 : Courier 28pt
  {
    Fl_Box *l = new Fl_Box(10, 104, 150, 40, "Tres grand (28pt) :");
    l->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    Fl_Time_Input *ti = new Fl_Time_Input(165, 100, 340, 48);
    ti->labelfont(FL_COURIER);
    ti->labelsize(28);
    ti->set_time(3, 45, 15, true);
    ti->callback(time_cb);
  }

  info_box = new Fl_Box(10, 165, 500, 24, "Utilisez Tab/fleches ou le selecteur horloge");
  info_box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

  Fl_Box *help = new Fl_Box(10, 196, 500, 55,
                            "Navigation : clic sur segment pour l'activer\n"
                            "Tab / <- -> pour changer  |  haut/bas ou molette pour incrementer\n"
                            "Bouton horloge pour le selecteur popup (police synchronisee)");
  help->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_TOP);

  win->end();
  win->resizable(win);
  win->show();
  return Fl::run();
}

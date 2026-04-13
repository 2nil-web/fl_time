// main.cpp
// Compilation :
//   g++ main.cpp Fl_Time_Picker.cpp Fl_Time_Input.cpp \
//       -o time_demo $(fltk-config --cxxflags --ldflags)
#include "Fl_Time_Input.H"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>

static Fl_Box *info_box = nullptr;

static void time_cb(Fl_Widget *w, void *)
{
  auto *t = (Fl_Time_Input *)w;
  char buf[64];
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d  (24h: %02d)  %s", t->hour(), t->minute(), t->second(), t->millis(), t->hour24(), t->is_pm() ? "PM" : "AM");
  info_box->copy_label(buf);
}

static Fl_Time_Input *make_input(int x, int y, Fl_Font font, int size, bool use24, bool show_ms, int hh, int mm, int ss, int ms, bool pm)
{
  Fl_Time_Input *ti = new Fl_Time_Input(x, y, 1, 1, use24, show_ms);
  ti->labelfont(font);
  ti->labelsize(size);
  ti->fit();
  ti->set_time(hh, mm, ss, ms, pm);
  ti->callback(time_cb);
  return ti;
}

int main()
{
  Fl_Window *win = new Fl_Window(10, 10, "sizing");
  win->begin();

  int lw = 200;

  // Ligne 1 : 12h, sans ms, Helvetica 14pt
  Fl_Time_Input *t1 = make_input(lw + 10, 10, FL_HELVETICA, 14, false, false, 10, 30, 0, 0, false);
  auto *l1 = new Fl_Box(10, 10, lw, t1->h(), "12h, sans ms (14pt):");
  l1->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

  // Ligne 2 : 24h, sans ms, Helvetica Bold 20pt
  int y2 = t1->y() + t1->h() + 8;
  Fl_Time_Input *t2 = make_input(lw + 10, y2, FL_HELVETICA_BOLD, 20, true, false, 14, 0, 0, 0, false);
  auto *l2 = new Fl_Box(10, y2, lw, t2->h(), "24h, sans ms (20pt):");
  l2->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

  // Ligne 3 : 12h, avec ms, Courier 14pt
  int y3 = t2->y() + t2->h() + 8;
  Fl_Time_Input *t3 = make_input(lw + 10, y3, FL_COURIER, 14, false, true, 3, 45, 15, 123, true);
  auto *l3 = new Fl_Box(10, y3, lw, t3->h(), "12h, avec ms (14pt):");
  l3->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

  // Ligne 4 : 24h, avec ms, Courier 20pt
  int y4 = t3->y() + t3->h() + 8;
  Fl_Time_Input *t4 = make_input(lw + 10, y4, FL_COURIER, 20, true, true, 23, 59, 59, 999, false);
  auto *l4 = new Fl_Box(10, y4, lw, t4->h(), "24h, avec ms (20pt):");
  l4->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

  int win_w = lw + 10 + t4->w() + 10;
  int iy = t4->y() + t4->h() + 12;

  info_box = new Fl_Box(10, iy, win_w - 20, 22, "Cliquez ou utilisez Tab/fleches/chiffres");
  info_box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

  auto *help = new Fl_Box(10, iy + 28, win_w - 20, 56,
                          "Ctrl+C : copier la valeur  |  Ctrl+V : coller\n"
                          "Chiffres : saisie gauche->droite avec avance auto\n"
                          "Horloge : selecteur popup (clic exterieur = annuler)");
  help->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_TOP);

  win->end();
  win->size(win_w, iy + 28 + 62);
  win->label("Fl_Time_Input");
  win->resizable(nullptr);
  win->show();
  return Fl::run();
}

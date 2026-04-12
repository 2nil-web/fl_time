// ============================================================
// Fl_Time_Input.cpp
// ============================================================
#include "Fl_Time_Input.H"
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <cstdio>

// ---- Constructeur / destructeur ----------------------------

Fl_Time_Input::Fl_Time_Input(int X, int Y, int W, int H, const char *lbl) : Fl_Group(X, Y, W, H, lbl), h_(12), m_(0), s_(0), pm_(false), active_seg_(SEG_NONE), picker_(nullptr), user_cb_(nullptr), user_data_(nullptr)
{
  end();
  box(FL_NO_BOX);
}

Fl_Time_Input::~Fl_Time_Input()
{
  delete picker_;
}

// ---- Valeur ------------------------------------------------

void Fl_Time_Input::set_time(int hh, int mm, int ss, bool p)
{
  h_ = hh;
  m_ = mm;
  s_ = ss;
  pm_ = p;
  redraw();
}

// ---- Callback ----------------------------------------------

void Fl_Time_Input::callback(Fl_Callback *cb, void *data)
{
  user_cb_ = cb;
  user_data_ = data;
}

void Fl_Time_Input::fire_callback()
{
  if (user_cb_)
    user_cb_(this, user_data_);
}

// ---- Police ------------------------------------------------

void Fl_Time_Input::measure_font()
{
  fm_.measure(labelfont(), labelsize());
}

// Taille idéale dérivée strictement des métriques de police.
// Largeur : pad + HH + sep + MM + sep + SS + pad + AMPM + spin + clock
// Hauteur : fm_.ch  (une ligne de texte avec ses marges)
void Fl_Time_Input::preferred_size(int &pw, int &ph) const
{
  // On recrée des métriques locales (const, pas de modification de fm_)
  TimeFontMetrics m;
  m.measure(labelfont(), labelsize());

  int spin_w = m.ch / 2 + 2; // bouton spin
  int clock_w = m.ch;        // bouton horloge (carré)

  pw = m.pad + m.cw_num + m.sep_w // HH :
       + m.cw_num + m.sep_w       // MM :
       + m.cw_num + m.pad         // SS (espace)
       + m.cw_ampm                // AM/PM
       + spin_w + clock_w;        // boutons

  ph = m.ch;
}

void Fl_Time_Input::fit()
{
  int pw, ph;
  preferred_size(pw, ph);
  Fl_Group::resize(x(), y(), pw, ph);
}

void Fl_Time_Input::resize(int X, int Y, int W, int H)
{
  // La position est libre, mais on impose toujours preferred_size()
  // pour figer les dimensions quoi qu'en demande le layout parent.
  int pw, ph;
  preferred_size(pw, ph);
  (void)W;
  (void)H; // dimensions demandées ignorées
  Fl_Group::resize(X, Y, pw, ph);
}

// ---- Géométrie ---------------------------------------------

void Fl_Time_Input::compute_rects()
{
  measure_font();

  // Hauteur de référence : fm_.ch (dérivée de la police),
  // centrée verticalement dans h() si le widget est plus grand.
  int rh = fm_.ch;
  int ry = y() + (h() - rh) / 2;

  int clock_w = rh;
  int spin_w = rh / 2 + 2;

  r_.cw = clock_w;
  r_.ch = rh;
  r_.cx = x() + w() - clock_w;
  r_.cy = ry;
  r_.sw = spin_w;
  r_.sh = rh;
  r_.sx = r_.cx - spin_w;
  r_.sy = ry;
  r_.fx = x();
  r_.fy = ry;
  r_.fw = r_.sx - x();
  r_.fh = rh;

  int pad = fm_.pad;
  int px = r_.fx + pad;

  r_.seg_x[SEG_HOUR] = px;
  r_.seg_w[SEG_HOUR] = fm_.cw_num;
  px += fm_.cw_num + fm_.sep_w;
  r_.seg_x[SEG_MIN] = px;
  r_.seg_w[SEG_MIN] = fm_.cw_num;
  px += fm_.cw_num + fm_.sep_w;
  r_.seg_x[SEG_SEC] = px;
  r_.seg_w[SEG_SEC] = fm_.cw_num;
  px += fm_.cw_num + pad;
  r_.seg_x[SEG_AMPM] = px;
  r_.seg_w[SEG_AMPM] = fm_.cw_ampm;
}

// ---- Dessin ------------------------------------------------

void Fl_Time_Input::draw_segment(const char *txt, int sx, int sy, int sw, int sh, bool active)
{
  if (active)
  {
    fl_color(0x0078d4ff);
    fl_rectf(sx, sy + 1, sw, sh - 2);
    fl_color(FL_WHITE);
  }
  else
  {
    fl_color(FL_FOREGROUND_COLOR);
  }
  fl_font(fm_.font, fm_.size);
  fl_draw(txt, sx, sy, sw, sh, FL_ALIGN_CENTER);
}

void Fl_Time_Input::draw_spin_button(int bx, int by, int bw, int bh)
{
  fl_draw_box(FL_BORDER_BOX, bx, by, bw, bh, FL_BACKGROUND_COLOR);
  int mx = bx + bw / 2;
  int aw = bw / 3, am = bh / 5;
  fl_color(FL_DARK3);
  fl_polygon(mx - aw, by + bh / 2 - 1, mx + aw, by + bh / 2 - 1, mx, by + am);
  fl_polygon(mx - aw, by + bh / 2 + 1, mx + aw, by + bh / 2 + 1, mx, by + bh - am);
}

void Fl_Time_Input::draw_clock_button(int bx, int by, int bw, int bh)
{
  fl_draw_box(FL_BORDER_BOX, bx, by, bw, bh, FL_BACKGROUND_COLOR);
  int cx = bx + bw / 2, cy = by + bh / 2;
  int cr = bh / 3;
  fl_color(FL_DARK3);
  fl_circle(cx, cy, cr);
  fl_line(cx, cy, cx, cy - cr + cr / 4);
  fl_line(cx, cy, cx + cr - cr / 4, cy);
}

void Fl_Time_Input::draw()
{
  compute_rects();

  fl_draw_box(FL_BORDER_BOX, r_.fx, r_.fy, r_.fw, r_.fh, FL_WHITE);

  // Séparateurs ":"
  fl_font(fm_.font, fm_.size);
  fl_color(FL_DARK3);
  fl_draw(":", r_.seg_x[SEG_HOUR] + r_.seg_w[SEG_HOUR], r_.fy, fm_.sep_w, r_.fh, FL_ALIGN_CENTER);
  fl_draw(":", r_.seg_x[SEG_MIN] + r_.seg_w[SEG_MIN], r_.fy, fm_.sep_w, r_.fh, FL_ALIGN_CENTER);

  char buf[4];
  snprintf(buf, sizeof(buf), "%02d", h_);
  draw_segment(buf, r_.seg_x[SEG_HOUR], r_.fy, r_.seg_w[SEG_HOUR], r_.fh, active_seg_ == SEG_HOUR);

  snprintf(buf, sizeof(buf), "%02d", m_);
  draw_segment(buf, r_.seg_x[SEG_MIN], r_.fy, r_.seg_w[SEG_MIN], r_.fh, active_seg_ == SEG_MIN);

  snprintf(buf, sizeof(buf), "%02d", s_);
  draw_segment(buf, r_.seg_x[SEG_SEC], r_.fy, r_.seg_w[SEG_SEC], r_.fh, active_seg_ == SEG_SEC);

  draw_segment(pm_ ? "PM" : "AM", r_.seg_x[SEG_AMPM], r_.fy, r_.seg_w[SEG_AMPM], r_.fh, active_seg_ == SEG_AMPM);

  draw_spin_button(r_.sx, r_.sy, r_.sw, r_.sh);
  draw_clock_button(r_.cx, r_.cy, r_.cw, r_.ch);
  draw_label();
}

// ---- Navigation entre segments -----------------------------

void Fl_Time_Input::activate_segment(TimeSegment s)
{
  active_seg_ = s;
  redraw();
}

void Fl_Time_Input::next_segment()
{
  int n = (int)active_seg_;
  if (n == (int)SEG_NONE)
    n = -1;
  active_seg_ = (TimeSegment)((n + 1) % 4);
  redraw();
}

void Fl_Time_Input::prev_segment()
{
  int n = (int)active_seg_;
  if (n <= 0)
    n = 4;
  active_seg_ = (TimeSegment)(n - 1);
  redraw();
}

void Fl_Time_Input::increment(int delta)
{
  switch (active_seg_)
  {
  case SEG_HOUR:
    h_ += delta;
    if (h_ > 12)
      h_ = 1;
    if (h_ < 1)
      h_ = 12;
    break;
  case SEG_MIN:
    m_ = (m_ + delta + 60) % 60;
    break;
  case SEG_SEC:
    s_ = (s_ + delta + 60) % 60;
    break;
  case SEG_AMPM:
    pm_ = !pm_;
    break;
  default:
    break;
  }
  redraw();
  fire_callback();
}

// ---- Popup picker ------------------------------------------

void Fl_Time_Input::open_picker()
{
  measure_font();

  if (!picker_)
  {
    picker_ = new Fl_Time_Picker();
    picker_->on_accept = [this](Fl_Time_Picker::Time t) {
      h_ = t.h;
      m_ = t.m;
      s_ = t.s;
      pm_ = t.pm;
      redraw();
      fire_callback();
    };
  }

  picker_->set_metrics(fm_);

  int screen_x = window()->x() + x();
  int screen_y = window()->y() + y() + h();
  if (screen_y + picker_->h() > Fl::h())
    screen_y = window()->y() + y() - picker_->h();

  picker_->show_at(screen_x, screen_y, {h_, m_, s_, pm_});
}

// ---- Événements --------------------------------------------

int Fl_Time_Input::handle(int ev)
{
  compute_rects();

  switch (ev)
  {
  case FL_PUSH: {
    take_focus();
    int mx = Fl::event_x(), my = Fl::event_y();

    // Bouton horloge
    if (mx >= r_.cx && mx <= r_.cx + r_.cw && my >= r_.cy && my <= r_.cy + r_.ch)
    {
      open_picker();
      return 1;
    }
    // Bouton spin
    if (mx >= r_.sx && mx <= r_.sx + r_.sw && my >= r_.sy && my <= r_.sy + r_.sh)
    {
      if (active_seg_ == SEG_NONE)
        active_seg_ = SEG_HOUR;
      increment(my < r_.sy + r_.sh / 2 ? 1 : -1);
      return 1;
    }
    // Clic sur un segment
    for (int i = 0; i < 4; i++)
    {
      if (mx >= r_.seg_x[i] && mx <= r_.seg_x[i] + r_.seg_w[i])
      {
        activate_segment((TimeSegment)i);
        return 1;
      }
    }
    return 1;
  }

  case FL_MOUSEWHEEL:
    if (active_seg_ != SEG_NONE)
    {
      increment(-Fl::event_dy());
      return 1;
    }
    return 0;

  case FL_KEYDOWN: {
    int key = Fl::event_key();
    if (key == FL_Up)
    {
      if (active_seg_ == SEG_NONE)
        active_seg_ = SEG_HOUR;
      increment(1);
      return 1;
    }
    if (key == FL_Down)
    {
      if (active_seg_ == SEG_NONE)
        active_seg_ = SEG_HOUR;
      increment(-1);
      return 1;
    }
    if (key == FL_Tab && !Fl::event_shift())
    {
      next_segment();
      return 1;
    }
    if (key == FL_Tab && Fl::event_shift())
    {
      prev_segment();
      return 1;
    }
    if (key == FL_Right)
    {
      next_segment();
      return 1;
    }
    if (key == FL_Left)
    {
      prev_segment();
      return 1;
    }
    if (key >= '0' && key <= '9')
    {
      int d = key - '0';
      if (active_seg_ == SEG_HOUR)
        h_ = (d == 0 ? 12 : d);
      if (active_seg_ == SEG_MIN)
        m_ = d * 10;
      if (active_seg_ == SEG_SEC)
        s_ = d * 10;
      redraw();
      fire_callback();
      return 1;
    }
    if (key == 'a' || key == 'A')
    {
      pm_ = false;
      redraw();
      fire_callback();
      return 1;
    }
    if (key == 'p' || key == 'P')
    {
      pm_ = true;
      redraw();
      fire_callback();
      return 1;
    }
    break;
  }

  case FL_FOCUS:
    if (active_seg_ == SEG_NONE)
      active_seg_ = SEG_HOUR;
    redraw();
    return 1;

  case FL_UNFOCUS:
    active_seg_ = SEG_NONE;
    redraw();
    return 1;
  }

  return Fl_Group::handle(ev);
}

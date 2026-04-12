// ============================================================
// Fl_Time_Picker.cpp
// ============================================================
#include "Fl_Time_Picker.H"
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <cstdio>

// ---- Constructeur ------------------------------------------

Fl_Time_Picker::Fl_Time_Picker() : Fl_Window(1, 1)
{
  set_non_modal();
  border(0);
  box(FL_BORDER_BOX);
  color(FL_WHITE);
  cur_ = {12, 0, 0, false};
  for (int i = 0; i < 4; i++)
    sc_[i] = 0;
  fm_.measure(FL_HELVETICA, FL_NORMAL_SIZE);
  layout();
  end();
}

// ---- Police ------------------------------------------------

void Fl_Time_Picker::set_metrics(const TimeFontMetrics &m)
{
  fm_ = m;
  layout();
}

// ---- Disposition -------------------------------------------

void Fl_Time_Picker::layout()
{
  col_w_[0] = fm_.cw_num;
  col_w_[1] = fm_.cw_num;
  col_w_[2] = fm_.cw_num;
  col_w_[3] = fm_.cw_ampm;

  header_h_ = fm_.ch;
  // Footer réduit : bouton OK seul
  btn_h_ = fm_.ch;
  btn_w_ = (int)(fm_.cw_ampm * 1.4);
  footer_h_ = btn_h_ + fm_.pad * 2;

  int total_w = col_w_[0] + col_w_[1] + col_w_[2] + col_w_[3];
  int total_h = header_h_ + ROWS_V * fm_.ch + footer_h_;
  size(total_w, total_h);
}

// ---- Scroll ------------------------------------------------

void Fl_Time_Picker::clamp_scroll(int col)
{
  int cnt = col_count(col);
  sc_[col] = ((sc_[col] % cnt) + cnt) % cnt;
}

void Fl_Time_Picker::update_cur_from_scroll()
{
  cur_.h = sc_[0] + 1;
  cur_.m = sc_[1];
  cur_.s = sc_[2];
  cur_.pm = (sc_[3] == 1);
}

// ---- Validation / annulation -------------------------------

void Fl_Time_Picker::accept_and_hide()
{
  Fl::grab(nullptr); // relâche le grab avant tout appel utilisateur
  if (on_accept)
    on_accept(cur_);
  hide();
}

void Fl_Time_Picker::cancel_and_hide()
{
  Fl::grab(nullptr);
  hide();
}

// ---- Géométrie colonnes ------------------------------------

int Fl_Time_Picker::col_x(int c) const
{
  int x = 0;
  for (int i = 0; i < c; i++)
    x += col_w_[i];
  return x;
}

int Fl_Time_Picker::col_at(int mx) const
{
  for (int c = 0; c < 4; c++)
    if (mx < col_x(c) + col_w_[c])
      return c;
  return 3;
}

void Fl_Time_Picker::item_label(int col, int idx, char *buf, int bufsz) const
{
  if (col == 3)
    snprintf(buf, bufsz, "%s", idx == 0 ? "AM" : "PM");
  else
    snprintf(buf, bufsz, "%02d", col == 0 ? idx + 1 : idx);
}

// ---- Dessin ------------------------------------------------

void Fl_Time_Picker::draw_col(int col, int cx)
{
  int cnt = col_count(col);
  int sel = sc_[col];
  int center = ROWS_V / 2;
  int cw = col_w_[col];

  fl_push_clip(cx, clip_top(), cw, ROWS_V * fm_.ch);

  fl_color(0x0078d4ff);
  fl_rectf(cx, clip_top() + center * fm_.ch, cw, fm_.ch);

  fl_font(fm_.font, fm_.size);
  char buf[8];
  for (int row = 0; row < ROWS_V; row++)
  {
    int item_idx = ((sel - center + row) % cnt + cnt) % cnt;
    int iy = clip_top() + row * fm_.ch;
    fl_color(row == center ? FL_WHITE : FL_BLACK);
    item_label(col, item_idx, buf, sizeof(buf));
    fl_draw(buf, cx, iy, cw, fm_.ch, FL_ALIGN_CENTER);
  }

  fl_pop_clip();
}

void Fl_Time_Picker::draw()
{
  fl_draw_box(FL_BORDER_BOX, 0, 0, w(), h(), FL_WHITE);

  // En-têtes
  const char *hdrs[] = {"H", "Min", "Sec", "AM/PM"};
  fl_font(fm_.font, fm_.size);
  fl_color(FL_DARK3);
  for (int c = 0; c < 4; c++)
  {
    int cx = col_x(c);
    fl_push_clip(cx, 0, col_w_[c], header_h_);
    fl_draw(hdrs[c], cx, 0, col_w_[c], header_h_, FL_ALIGN_CENTER);
    fl_pop_clip();
  }

  fl_color(fl_lighter(FL_GRAY));
  fl_line(0, header_h_ - 1, w(), header_h_ - 1);

  for (int c = 1; c < 4; c++)
  {
    int sx = col_x(c);
    fl_line(sx, 0, sx, h() - footer_h_);
  }

  for (int c = 0; c < 4; c++)
    draw_col(c, col_x(c));

  // Séparateur au-dessus du footer
  int fy = h() - footer_h_;
  fl_color(fl_lighter(FL_GRAY));
  fl_line(0, fy, w(), fy);

  // Bouton OK centré
  int bx = ok_x(), by = btn_y();
  fl_draw_box(FL_FLAT_BOX, bx, by, btn_w_, btn_h_, 0x0078d4ff);
  fl_font(fm_.font, fm_.size);
  fl_color(FL_WHITE);
  fl_draw("OK", bx, by, btn_w_, btn_h_, FL_ALIGN_CENTER);
}

// ---- Événements --------------------------------------------

int Fl_Time_Picker::handle(int ev)
{
  switch (ev)
  {
  case FL_PUSH: {
    int mx = Fl::event_x(), my = Fl::event_y();

    // Clic en dehors de la fenêtre → annulation
    // (possible grâce au grab : tous les FL_PUSH arrivent ici)
    if (mx < 0 || mx >= w() || my < 0 || my >= h())
    {
      cancel_and_hide();
      return 1;
    }

    // Bouton OK
    {
      int bx = ok_x(), by = btn_y();
      if (my >= by && my <= by + btn_h_ && mx >= bx && mx <= bx + btn_w_)
      {
        accept_and_hide();
        return 1;
      }
    }

    // Clic dans la zone colonnes
    if (my >= clip_top() && my < clip_bottom())
    {
      int col = col_at(mx);
      int delta = (my - clip_top()) / fm_.ch - ROWS_V / 2;
      sc_[col] += delta;
      clamp_scroll(col);
      update_cur_from_scroll();
      redraw();
      return 1;
    }
    return 1;
  }

  case FL_MOUSEWHEEL:
    if (Fl::event_y() >= clip_top() && Fl::event_y() < clip_bottom())
    {
      int col = col_at(Fl::event_x());
      sc_[col] += Fl::event_dy();
      clamp_scroll(col);
      update_cur_from_scroll();
      redraw();
      return 1;
    }
    return 0;

  case FL_KEYDOWN:
    if (Fl::event_key() == FL_Escape)
    {
      cancel_and_hide();
      return 1;
    }
    if (Fl::event_key() == FL_Enter)
    {
      accept_and_hide();
      return 1;
    }
    break;
  }

  return Fl_Window::handle(ev);
}

// ---- Affichage ---------------------------------------------

void Fl_Time_Picker::show_at(int screen_x, int screen_y, Time t)
{
  sc_[0] = t.h - 1;
  sc_[1] = t.m;
  sc_[2] = t.s;
  sc_[3] = t.pm ? 1 : 0;
  update_cur_from_scroll();
  position(screen_x, screen_y);
  show();
  // Fl::grab() redirige tous les FL_PUSH vers cette fenêtre,
  // même ceux qui se produisent en dehors d'elle.
  Fl::grab(*this);
  redraw();
}


#include <FL/Fl.H>
#include <FL/Fl_SVG_Image.H>
#include <FL/fl_ask.H>

#include "Fl_Time.h"
#include "fl_time_test_icon.h"

#define SIMPLE_CB [](Fl_Widget *, void *)->void
int main()
{
  make_window();
  fl_message_title_default("Fl_time_test");
  Fl_SVG_Image svg(0, svg_data);
  main_window->icon((Fl_RGB_Image *)&svg);

  main_window->show();
  fl_message_position(main_window->x_root(), main_window->y_root() + 100, 0);

  Fl::flush();
  return Fl::run();
}

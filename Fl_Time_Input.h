#ifndef FL_TIME_INPUT_H
#define FL_TIME_INPUT_H

#include <iostream>
#include <regex>
#include <string>

// To avoid warning C5260 in FL/Enumerations.H with Visual Studio.
// See from line 989 and around 70 that follow in the Enumeration.h file of fltk 1.4
#ifdef _MSVC_LANG
#pragma warning(push)
#pragma warning(disable : 5260)
#include <FL/Fl.H>
#pragma warning(pop)
#endif
#include <FL/Fl_Input.H>

class Fl_Time_Input : public Fl_Input
{
private:
  static void real_callback(Fl_Widget *, void *);
  Fl_Callback *user_cb;
  void *user_cb_v;

  std::regex time_regexp;
  std::string val;
  bool format_callback(void *);

  bool limit_24h;
  void Init();
  std::string check_actual_time(std::string, std::string);

public:
  char ms_sep;
  void Init(char, bool);

  Fl_Time_Input(int x, int y, int w, int h, const char *label = nullptr, char _ms_sep = 0, bool _limit_24h = false);

  void set_24h_limit(bool);
  void callback(Fl_Callback *, void *v = nullptr);
  const char *value()
  {
    return ((Fl_Input *)this)->value();
  }
  int value(const char *);
  int value(const std::string);
};
#endif /* FL_TIME_INPUT_H */

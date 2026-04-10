
#include <iostream>
#include <string>

#include "Fl_Time_Input.h"
#include "utils.h"

// A timer input with the format HH:MM:SS.SSS
// Milliseconds are separated from seconds by a comma (default) or a dot
// The maximum display time is by default, up to 99 hours, 99 minutes, and 99.999 seconds.
// But it can be toggled between 24 or 99 hours by using the set_24h_limit(true/false) method

std::string Fl_Time_Input::check_actual_time(std::string old_val, std::string act_val)
{
  if (act_val.size() < old_val.size())
    act_val += old_val.substr(act_val.size());
  if (act_val.size() > 12)
    act_val.erase(12);
  std::string new_val = act_val;

  for (size_t i = 0; i < new_val.size(); i++)
  {
    switch (i)
    {
    case 0:
    case 1:
      if (std::isdigit(new_val[i]))
      {
        if (limit_24h)
        {
          if (new_val[0] > '2')
            new_val[0] = '2';
          if (new_val[1] > '3')
            new_val[1] = '3';
        }
      }
      else
        new_val[i] = old_val[i];
      break;
    case 2:
    case 5:
      if (new_val[i] != ':')
        new_val[i] = ':';
      break;
    case 3:
    case 6:
      if (std::isdigit(new_val[i]))
      {
        if (new_val[i] > '5')
          new_val[i] = '5';
      }
      else
        new_val[i] = old_val[i];
      break;
    case 8:
      if (new_val[i] != ms_sep)
        new_val[i] = ms_sep;
      break;
    default: // 4, 7, 9, 10, 11
      if (!std::isdigit(new_val[i]))
        new_val[i] = old_val[i];
      break;
    }
  }

  return new_val;
}

// Check and eventually correct the input
// Return true if value has changed
bool Fl_Time_Input::format_callback(void *v)
{
  if (v == nullptr)
    return false;
  std::string &old_val = *(static_cast<std::string *>(v));

  int old_pos = insert_position();
  int new_pos = old_pos;
  std::string ins_txt = Fl::event_text();
  std::string act_val = strict_replace(old_val, ins_txt, old_pos - ins_txt.size());
  std::string new_val = "";

  if (std::regex_match(act_val, time_regexp))
  {
    new_val = act_val;
  }
  else
  {
    new_val = check_actual_time(old_val, act_val);

    switch (old_pos)
    {
    case 3:
    case 6:
      if (isdigit(act_val[old_pos - 1]))
      {
        new_val[old_pos] = act_val[old_pos - 1];
        new_val[old_pos - 1] = ':';
        new_pos++;
      }
      break;
    case 9:
      if (isdigit(act_val[old_pos - 1]))
      {
        new_val[old_pos] = act_val[old_pos - 1];
        new_val[old_pos - 1] = ms_sep;
        new_pos++;
      }
      break;
    default:
      if (std::isdigit(act_val[old_pos - 1]))
      {
        if (old_pos <= (int)old_val.size())
          new_val[old_pos - 1] = act_val[old_pos - 1];
        else
          new_val[(int)old_val.size() - 1] = act_val[old_pos - 1];
      }
      else
        new_pos--;
      break;
    }

    // Hour limit to 24 or 99
    if (limit_24h)
    {
      if (new_pos == 1 && new_val[0] > '2')
        new_val[0] = '2';
      if (new_pos == 2 && new_val[1] > '3')
        new_val[1] = '3';
    }

    if ((new_pos == 4 || new_pos == 7) && new_val[new_pos - 1] > '5')
      new_val[new_pos - 1] = '5';
  }

  bool has_changed = (old_val != new_val);

  if (has_changed)
    std::cout << "old_val: " << old_val << ", ins_txt: " << ins_txt<< ", act_val: " << act_val << ", new_val: " << new_val << ", old_pos: " << new_pos << ", new_pos: " << new_pos << std::endl;

  if (new_val.empty())
    new_val = old_val;

  old_val = new_val;
  value(new_val.c_str());
  if (new_pos > (int)new_val.size())
    new_pos = (int)new_val.size();
  insert_position(new_pos);

  val = new_val;
  return has_changed;
}

void Fl_Time_Input::real_callback(Fl_Widget *w, void *v)
{
  Fl_Time_Input *me = (Fl_Time_Input *)w;

  if (me->format_callback(v))
  {
    if (me->user_cb != nullptr)
    {
      me->user_cb(w, me->user_cb_v);
    }
  }
}

void Fl_Time_Input::callback(Fl_Callback *_cb, void *_v)
{
  user_cb = _cb;
  user_cb_v = _v;
}

int Fl_Time_Input::value(const char *s)
{
  val = s;
  return ((Fl_Input *)this)->value(s);
}

int Fl_Time_Input::value(const std::string s)
{
  val = s;
  return ((Fl_Input *)this)->value(s.c_str());
}

void Fl_Time_Input::Init()
{
  val = "00:00:00" + std::string(1, ms_sep) + "000";
  if (limit_24h)
    time_regexp = std::regex(R"(^(?:[01]\d|2[0-3]):[0-5]\d:[0-5]\d[.,]\d{3}$)");
  else
    time_regexp = std::regex(R"(\d\d:[0-5]\d:[0-5]\d[.,]\d{3})");
  value(val.c_str());
  ((Fl_Input *)this)->callback(real_callback, static_cast<void *>(&val));
}

void Fl_Time_Input::Init(char _ms_sep, bool _limit_24h) //, int _time_type)
{
  if (_ms_sep == 0)
    ms_sep = ',';
  else
    ms_sep = _ms_sep;
  limit_24h = _limit_24h;
  user_cb = nullptr;
  user_cb_v = nullptr;
  Init();
}

Fl_Time_Input::Fl_Time_Input(int x, int y, int w, int h, const char *label, char _ms_sep, bool _limit_24h) : Fl_Input(x, y, w, h, label)
{
  Init(_ms_sep, _limit_24h);
}

void Fl_Time_Input::set_24h_limit(bool _limit_24h)
{
  if (_limit_24h != limit_24h)
  {
    limit_24h = _limit_24h;
    Init();
  }
}

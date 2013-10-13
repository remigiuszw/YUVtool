#ifndef SCROLL_ADAPTER_H
#define SCROLL_ADAPTER_H

#include <gtkmm.h>

class Scroll_adapter : public Gtk::ScrolledWindow {

public:
  Scroll_adapter();
  Gtk::Fixed &get_fixed();
  const Gtk::Fixed &get_fixed() const;

private:
  void on_scroll();

  Gtk::Fixed m_fixed;
};

#endif // SCROLL_ADAPTER_H

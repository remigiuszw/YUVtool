#include "scroll_adapter.h"
#include <iostream>

Scroll_adapter::Scroll_adapter() {
  get_hadjustment()->signal_changed().connect( sigc::mem_fun( *this,
    &Scroll_adapter::on_scroll ) );
  get_hadjustment()->signal_value_changed().connect( sigc::mem_fun( *this,
    &Scroll_adapter::on_scroll ) );
  get_vadjustment()->signal_changed().connect( sigc::mem_fun( *this,
    &Scroll_adapter::on_scroll ) );
  get_vadjustment()->signal_value_changed().connect( sigc::mem_fun( *this,
    &Scroll_adapter::on_scroll ) );

  add( m_fixed );
}
//------------------------------------------------------------------------------
Gtk::Fixed &Scroll_adapter::get_fixed() {
  return m_fixed;
}
//------------------------------------------------------------------------------
const Gtk::Fixed &Scroll_adapter::get_fixed() const {
  return m_fixed;
}
//------------------------------------------------------------------------------
void Scroll_adapter::on_scroll() {
  if( m_fixed.get_children().size() > 0 )
  {
    Gtk::Widget &adapted = *m_fixed.get_children().front();
    m_fixed.move( adapted, get_hadjustment()->get_value(),
      get_vadjustment()->get_value() );
    adapted.set_size_request( get_hadjustment()->get_page_size(),
      get_vadjustment()->get_page_size() );
  }
}

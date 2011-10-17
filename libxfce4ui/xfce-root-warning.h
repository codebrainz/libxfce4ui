/*
 * Copyright (c) 2011 Matthew Brush <matt@xfce.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef __XFCEROOTWARNING_H__
#define __XFCEROOTWARNING_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS


#define DISABLE_ROOT_CHECK 1  /* flip to 1 for testing as non-root */


#define XFCE_TYPE_ROOT_WARNING            (xfce_root_warning_get_type())
#define XFCE_ROOT_WARNING(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), XFCE_TYPE_ROOT_WARNING, XfceRootWarning))
#define XFCE_ROOT_WARNING_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), XFCE_TYPE_ROOT_WARNING, XfceRootWarningClass))
#define XFCE_IS_ROOT_WARNING(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), XFCE_TYPE_ROOT_WARNING))
#define XFCE_IS_ROOT_WARNING_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), XFCE_TYPE_ROOT_WARNING))
#define XFCE_ROOT_WARNING_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), XFCE_TYPE_ROOT_WARNING, XfceRootWarningClass))


typedef struct _XfceRootWarning         XfceRootWarning;
typedef struct _XfceRootWarningClass    XfceRootWarningClass;
typedef struct _XfceRootWarningPrivate  XfceRootWarningPrivate;


struct _XfceRootWarning
{
  GtkInfoBar  parent;
  XfceRootWarningPrivate *priv;
};


struct _XfceRootWarningClass
{
  GtkInfoBarClass parent_class;
};


GType           xfce_root_warning_get_type           (void);
GtkWidget      *xfce_root_warning_new                (void);
GtkWidget      *xfce_root_warning_new_with_label     (const gchar     *message);
gboolean        xfce_root_warning_auto_pack          (XfceRootWarning *warning,
                                                      GtkWindow       *window);
const gchar    *xfce_root_warning_get_message        (XfceRootWarning *warning);
void            xfce_root_warning_set_message        (XfceRootWarning *warning,
                                                      const gchar     *message);
void            xfce_root_warning_set_message_markup (XfceRootWarning *warning,
                                                      const gchar     *markup);
gboolean        xfce_root_warning_get_can_dismiss    (XfceRootWarning *warning);
void            xfce_root_warning_set_can_dismiss    (XfceRootWarning *warning,
                                                      gboolean         can_dismiss);
gboolean        xfce_root_warning_get_dismissed      (XfceRootWarning *warning);
void            xfce_root_warning_set_dismissed      (XfceRootWarning *warning,
                                                      gboolean         dismissed);
gboolean        xfce_root_warning_get_activated      (XfceRootWarning *warning);
gboolean        xfce_root_warning_get_show_icon      (XfceRootWarning *warning);
void            xfce_root_warning_set_show_icon      (XfceRootWarning *warning,
                                                      gboolean         show_icon);
const GdkColor *xfce_root_warning_get_fg_color       (XfceRootWarning *warning);
void            xfce_root_warning_set_fg_color       (XfceRootWarning *warning,
                                                      const GdkColor  *fg_color);
const GdkColor *xfce_root_warning_get_bg_color       (XfceRootWarning *warning);
void            xfce_root_warning_set_bg_color       (XfceRootWarning *warning,
                                                      const GdkColor  *bg_color);
guint           xfce_root_warning_get_padding        (XfceRootWarning *warning);
void            xfce_root_warning_set_padding        (XfceRootWarning *warning,
                                                      guint            padding);


G_END_DECLS

#endif /* __XFCEROOTWARNING_H__ */

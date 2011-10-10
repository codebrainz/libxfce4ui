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

/**
 * SECTION:xfce-root-warning
 * @short_description: Shows a highly visible warning message when the
 * application is run as root.
 * @title: XfceRootWarning
 * @stability: Unstable
 * @include: libxfce4ui/libxfce4ui.h
 *
 * This widget is meant to be re-used in all XFCE programs that wish
 * to alert and remind the user that they're running the application
 * as the root user on Unix-like systems or with elevated priviledges
 * on Windows systems.
 *
 * This widget should look quite familiar to XFCE users since it's
 * based on the existing warnings found in applications such as
 * Thunar and Mousepad.  The #XfceRootWarning can be added to an
 * existing program, using the default values, in a single line of
 * code (see examples below).  The default values are meant to closely
 * mimic the appearance of the existing root warning messages in popular
 * XFCE applications.
 *
 * The inspiration for this widget is two fold:
 *
 * <itemizedlist>
 *   <listitem>
 *     <para>
 *       To provide a single easy to use widget to avoid code
 *       duplication and to ensure a consistent appearance of the root
 *       warning messages in XFCE applications.
 *     </para>
 *   </listitem>
 *   <listitem>
 *     <para>
 *       To provide the ability for GTK+ themes to override the colour
 *       of these warning widgets to make them integrate better into
 *       the overall appearance of the theme.
 *     </para>
 *   </listitem>
 * </itemizedlist>
 *
 * <example>
 *   <title>Basic usage</title>
 *   <programlisting>
 *     // pack it wherever it's needed into the program's UI
 *     gtk_box_pack_start (GTK_BOX (vbox),
 *                         xfce_root_warning_new (),
 *                         FALSE, TRUE, 0);
 *   </programlisting>
 * </example>
 *
 * <example>
 *   <title>Using the auto-packing feature</title>
 *   <programlisting>
 *     GtkWidget *warning = xfce_root_warning_new ();
 *
 *     if (!xfce_root_warning_auto_pack (XFCE_ROOT_WARNING (warning),
 *                                       GTK_WINDOW (window)))
 *       {
 *         // the XfceRootWarning couldn't be packed into the window
 *       }
 *   </programlisting>
 * </example>
 *
 * <example>
 *   <title>Connecting to the #XfceRootWarning::dismissed signal</title>
 *   <programlisting>
 *     static void
 *     on_root_warning_dismissed (XfceRootWarning *warning,
 *                                gpointer         user_data)
 *     {
 *       g_debug ("Root warning dismissed.");
 *     }
 *
 *     // ...
 *
 *     gint main (gint argc, gchar *argv[])
 *     {
 *       GtkWidget *root_warning;
 *
 *       // init GTK+ and create a window ...
 *
 *       root_warning = xfce_root_warning_new ();
 *
 *       g_signal_connect (root_warning,
 *                         "dismissed",
 *                         G_CALLBACK (on_root_warning_dismissed),
 *                         NULL);
 *
 *        // rest of code and run main loop ...
 *
 *        return 0;
 *      }
 *
 *   </programlisting>
 * </example>
 */


#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4ui/xfce-root-warning.h>


#ifdef G_OS_WIN32
/* not tested */
#  include <windows.h>
#  define AM_ROOT ((gboolean) IsUserAnAdmin())
#  define DEFAULT_MESSAGE \
     _("Warning!  You are using elevated priviledges, you may harm your system.")
#else
#  include <unistd.h>
#  define AM_ROOT ((gboolean) (getuid() == 0))
#  define DEFAULT_MESSAGE \
     _("Warning!  You are using the root account, you may harm your system.")
#endif


#ifndef USE_INFOBAR

/* defaults used if not over-ridden through resource file styles */
#define DEFAULT_BG_COLOR "#B4254B"
#define DEFAULT_FG_COLOR "#FEFEFE"
#define DEFAULT_PADDING  6

/* hold colours in the same units that cairo wants */
struct CairoColor
{
    gdouble red;
    gdouble green;
    gdouble blue;
};

#endif /* USE_INFOBAR */


struct _XfceRootWarningPrivate
{
  GtkWidget        *label;          /* the message label */
  GtkWidget        *dismiss_button; /* the little close button */
  GtkWidget        *warning_image;  /* the image used for the warning icon */
  gboolean          dismissed;      /* whether the warning has been dismissed */
  gboolean          can_dismiss;    /* whether the dismiss button is shown */
  gboolean          show_icon;      /* whether to show the warning icon */
#ifndef USE_INFOBAR
  GtkWidget        *hbox;           /* hbox that holds internal widgets */
  GdkColor          fg_color;       /* warning message text color */
  GdkColor          bg_color;       /* bg colour under the highlight gradient */
  struct CairoColor cairo_bg_color; /* stores bg_color as doubles */
  gboolean          highlight;      /* whether to draw gradient highlight */
  guint             padding;        /* spacing around internal widgets */
#endif
};


enum
{
  SIGNAL_DISMISSED,
  SIGNAL_CLICKED,
  SIGNAL_LAST
};


enum
{
  PROP_0,
  PROP_MESSAGE,
  PROP_CAN_DISMISS,
  PROP_ACTIVATED,
  PROP_SHOW_ICON,
#ifndef USE_INFOBAR
  PROP_FG_COLOR,
  PROP_BG_COLOR,
  PROP_HIGHLIGHT,
  PROP_PADDING,
#endif
  PROP_LAST
};


static guint signals[SIGNAL_LAST];


static void     xfce_root_warning_finalize       (GObject         *object);
static void     xfce_root_warning_show           (GtkWidget       *widget);
#ifndef USE_INFOBAR
static gboolean xfce_root_warning_expose         (GtkWidget       *widget,
                                                  GdkEventExpose  *expose);
#endif
static void     xfce_root_warning_create_ui      (XfceRootWarning *warning);
static void     xfce_root_warning_get_property   (GObject         *object,
                                                  guint            property_id,
                                                  GValue          *value,
                                                  GParamSpec      *pspec);
static void     xfce_root_warning_set_property   (GObject         *object,
                                                  guint            property_id,
                                                  const GValue    *value,
                                                  GParamSpec      *pspec);
static void     xfce_root_warning_button_clicked (GtkButton       *button,
                                                  XfceRootWarning *warning);

#ifdef USE_INFOBAR
G_DEFINE_TYPE (XfceRootWarning, xfce_root_warning, GTK_TYPE_INFO_BAR)
#else
G_DEFINE_TYPE (XfceRootWarning, xfce_root_warning, GTK_TYPE_EVENT_BOX)
#endif


static void
xfce_root_warning_class_init (XfceRootWarningClass *klass)
{
  GObjectClass   *gobject_class;
  GtkWidgetClass *widget_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = xfce_root_warning_finalize;
  gobject_class->get_property = xfce_root_warning_get_property;
  gobject_class->set_property = xfce_root_warning_set_property;

  widget_class = GTK_WIDGET_CLASS (klass);
  widget_class->show = xfce_root_warning_show;

#ifndef USE_INFOBAR
  widget_class->expose_event = xfce_root_warning_expose;
#endif

  /*
   * Signals
   */

  /**
   * XfceRootWarning::dismissed:
   * @warning: The #XfceRootWarning widget that received the signal.
   *
   * Gets emitted when the user presses the little close button to
   * dismiss the widget or when the #XfceRootWarning:dismissed property
   * is set to %TRUE.
   **/
  signals[SIGNAL_DISMISSED] =
    g_signal_new ("dismissed",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  /**
   * XfceRootWarning::clicked:
   * @warning: The #XfceRootWarning widget that received the signal.
   *
   * Gets emitted when the user clicks on the widget with the left
   * mouse button (similar to a #GtkButton).
   **/
  signals[SIGNAL_CLICKED] =
    g_signal_new ("clicked",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  /*
   * Properties
   */
  g_object_class_install_property (
    gobject_class,
    PROP_MESSAGE,
    g_param_spec_string ("message",
                         "Message",
                         "Warning message to show",
                         DEFAULT_MESSAGE,
                         G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

  g_object_class_install_property (
    gobject_class,
    PROP_CAN_DISMISS,
    g_param_spec_boolean ("can-dismiss",
                          "CanDismiss",
                          "Whether the user can dismiss the warning",
                          FALSE,
                          G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

  g_object_class_install_property (
    gobject_class,
    PROP_ACTIVATED,
    g_param_spec_boolean ("activated",
                          "Activated",
                          "Whether the user is root or not",
                          FALSE,
                          G_PARAM_READABLE));

  g_object_class_install_property (
    gobject_class,
    PROP_ACTIVATED,
    g_param_spec_boolean ("dismissed",
                          "Dismissed",
                          "Whether the warning was shown and then dismissed",
                          FALSE,
                          G_PARAM_READABLE));

  g_object_class_install_property (
    gobject_class,
    PROP_SHOW_ICON,
    g_param_spec_boolean ("show-icon",
                          "ShowIcon",
                          "Whether to show the warning icon to the "
                          "left of the text",
                          FALSE,
                          G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

#ifndef USE_INFOBAR

  g_object_class_install_property (
    gobject_class,
    PROP_BG_COLOR,
    g_param_spec_boxed ("fg-color",
                        "ForegroundColor",
                        "The color of the warning text",
                        GDK_TYPE_COLOR,
                        G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

  g_object_class_install_property (
    gobject_class,
    PROP_FG_COLOR,
    g_param_spec_boxed ("bg-color",
                        "BackgroundColor",
                        "The color of the widget's background",
                        GDK_TYPE_COLOR,
                        G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

  g_object_class_install_property (
    gobject_class,
    PROP_HIGHLIGHT,
    g_param_spec_boolean ("highlight",
                          "Highlight",
                          "Whether to draw a highlighting gradient "
                          "over the widget's background",
                          FALSE,
                          G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

  g_object_class_install_property (
    gobject_class,
    PROP_PADDING,
    g_param_spec_uint ("padding",
                       "Padding",
                       "Amount of padding around internal widgets",
                       0, 100, 6,
                       G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

  /*
   * Style properties
   */
  gtk_widget_class_install_style_property (
    widget_class,
    g_param_spec_string ("bg-color",
                         "BackgroundColor",
                         "The base color used in the widget",
                         DEFAULT_BG_COLOR,
                         G_PARAM_READABLE));

  gtk_widget_class_install_style_property (
    widget_class,
    g_param_spec_string ("fg-color",
                         "ForegroundColor",
                         "The color of the warning text",
                         DEFAULT_FG_COLOR,
                         G_PARAM_READABLE));

  gtk_widget_class_install_style_property (
    widget_class,
    g_param_spec_int ("padding",
                      "Padding",
                      "The padding between the edge and the internal widgets",
                      0, 100, DEFAULT_PADDING,
                      G_PARAM_READABLE));

#endif /* USE_INFOBAR */

  g_type_class_add_private ((gpointer)klass, sizeof(XfceRootWarningPrivate));
}


static void
xfce_root_warning_finalize (GObject *object)
{
  g_return_if_fail (XFCE_IS_ROOT_WARNING (object));
  G_OBJECT_CLASS (xfce_root_warning_parent_class)->finalize (object);
}



static void
xfce_root_warning_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  XfceRootWarning *warning = XFCE_ROOT_WARNING (object);

  switch (property_id)
    {
    case PROP_ACTIVATED:
      g_value_set_boolean (value, xfce_root_warning_get_activated (warning));
      break;
    case PROP_CAN_DISMISS:
      g_value_set_boolean (value, xfce_root_warning_get_can_dismiss (warning));
      break;
    case PROP_MESSAGE:
      g_value_set_static_string (value, xfce_root_warning_get_message (warning));
      break;
    case PROP_SHOW_ICON:
      g_value_set_boolean (value, xfce_root_warning_get_show_icon (warning));
      break;
#ifndef USE_INFOBAR
    case PROP_FG_COLOR:
      g_value_set_boxed (value, xfce_root_warning_get_fg_color (warning));
      break;
    case PROP_BG_COLOR:
      g_value_set_boxed (value, xfce_root_warning_get_bg_color (warning));
      break;
    case PROP_HIGHLIGHT:
      g_value_set_boolean (value, xfce_root_warning_get_highlight (warning));
      break;
    case PROP_PADDING:
      g_value_set_uint (value, xfce_root_warning_get_padding (warning));
      break;
#endif
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}


static void
xfce_root_warning_set_property (GObject      *object,
                                guint         property_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  XfceRootWarning *warning = XFCE_ROOT_WARNING (object);

  switch (property_id)
    {
    case PROP_CAN_DISMISS:
      xfce_root_warning_set_can_dismiss (warning, g_value_get_boolean (value));
      break;
    case PROP_MESSAGE:
      xfce_root_warning_set_message (warning, g_value_get_string (value));
      break;
    case PROP_SHOW_ICON:
      xfce_root_warning_set_show_icon (warning, g_value_get_boolean (value));
      break;
#ifndef USE_INFOBAR
    case PROP_FG_COLOR:
      xfce_root_warning_set_fg_color (warning, g_value_get_boxed (value));
      break;
    case PROP_BG_COLOR:
      xfce_root_warning_set_bg_color (warning, g_value_get_boxed (value));
      break;
    case PROP_HIGHLIGHT:
      xfce_root_warning_set_highlight (warning, g_value_get_boolean (value));
      break;
    case PROP_PADDING:
      xfce_root_warning_set_padding (warning, g_value_get_uint (value));
      break;
#endif
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}


static void
xfce_root_warning_show (GtkWidget *widget)
{
  gboolean         changed;
  XfceRootWarning *warning;

  g_return_if_fail (XFCE_IS_ROOT_WARNING (widget));

  warning = XFCE_ROOT_WARNING (widget);

  /* only ever show self if user is running as root (except for testing) */
  if (xfce_root_warning_get_activated (warning) || DISABLE_ROOT_CHECK)
    {
      changed = !warning->priv->dismissed;
      warning->priv->dismissed = FALSE;
      if (changed)
        g_object_notify (G_OBJECT (warning), "dismissed");
      GTK_WIDGET_CLASS (xfce_root_warning_parent_class)->show (widget);
    }
}


#ifndef USE_INFOBAR

static gboolean
xfce_root_warning_expose (GtkWidget      *widget,
                          GdkEventExpose *event)
{
  GdkWindow       *window;
  cairo_t         *cr;
  cairo_pattern_t *pat;
  XfceRootWarning *warning;

  g_return_val_if_fail (XFCE_IS_ROOT_WARNING (widget), FALSE);

  warning = XFCE_ROOT_WARNING (widget);
  window = gtk_widget_get_window (widget);

  /* create a new context to draw on from the widget's window */
  cr = gdk_cairo_create (GDK_DRAWABLE (window));

  /* set a clip region for the expose event */
  cairo_rectangle      (cr,
                        event->area.x,
                        event->area.y,
                        event->area.width,
                        event->area.height);
  cairo_clip (cr);

  /* rectangle the whole size of the widget */
  cairo_rectangle      (cr,
                        widget->allocation.x,
                        widget->allocation.y,
                        widget->allocation.width,
                        widget->allocation.height);

  /* set the bg colour from the widget's "bg-color" property */
  cairo_set_source_rgb (cr,
                        warning->priv->cairo_bg_color.red,
                        warning->priv->cairo_bg_color.green,
                        warning->priv->cairo_bg_color.blue);

  /* apply the colour to the widget's background */
  cairo_fill (cr);

  /* draw the highlighting gradient overlayed on the background */
  if (warning->priv->highlight)
    {
      pat = cairo_pattern_create_linear (0.0, 0.0, 0.0, widget->allocation.height);

      /* from white 20% at the top to white 0% at the centre */
      cairo_pattern_add_color_stop_rgba (pat, 0.0, 1.0, 1.0, 1.0, 0.20);
      cairo_pattern_add_color_stop_rgba (pat, 0.5, 1.0, 1.0, 1.0, 0.0);

      /* from black 0% at the centre to black 8% at the bottom */
      cairo_pattern_add_color_stop_rgba (pat, 0.5, 0.0, 0.0, 0.0, 0.0);
      cairo_pattern_add_color_stop_rgba (pat, 1.0, 0.0, 0.0, 0.0, 0.08);

      /* rectangle the whole size of the widget */
      cairo_rectangle  (cr,
                        widget->allocation.x,
                        widget->allocation.y,
                        widget->allocation.width,
                        widget->allocation.height);

      /* apply gradient */
      cairo_set_source (cr, pat);
      cairo_fill (cr);

      /* cleanup */
      cairo_pattern_destroy (pat);
    }

  /* cleanup */
  cairo_destroy (cr);

  return GTK_WIDGET_CLASS (xfce_root_warning_parent_class)->expose_event (widget, event);
}
#endif /* USE_INFOBAR */


static void
xfce_root_warning_init (XfceRootWarning *warning)
{
#ifndef USE_INFOBAR
  gchar    *bg_color = NULL;
  gchar    *fg_color = NULL;
  GdkColor  color_fg;
  GdkColor  color_bg;
  guint     padding;
#endif

  warning->priv = G_TYPE_INSTANCE_GET_PRIVATE (warning,
                                               XFCE_TYPE_ROOT_WARNING,
                                               XfceRootWarningPrivate);

  warning->priv->label          = NULL;
  warning->priv->dismiss_button = NULL;
  warning->priv->warning_image  = NULL;
  warning->priv->dismissed      = FALSE;
  warning->priv->can_dismiss    = FALSE;
  warning->priv->show_icon      = FALSE;

#ifndef USE_INFOBAR
  warning->priv->padding        = DEFAULT_PADDING;

  gdk_color_parse (DEFAULT_FG_COLOR, &warning->priv->fg_color);
  gdk_color_parse (DEFAULT_BG_COLOR, &warning->priv->bg_color);

  warning->priv->cairo_bg_color.red   = (gdouble) ((gdouble) warning->priv->bg_color.red / (gdouble) G_MAXUINT16);
  warning->priv->cairo_bg_color.green = (gdouble) ((gdouble) warning->priv->bg_color.green / (gdouble) G_MAXUINT16);
  warning->priv->cairo_bg_color.blue  = (gdouble) ((gdouble) warning->priv->bg_color.blue / (gdouble) G_MAXUINT16);
#endif

  xfce_root_warning_create_ui (warning);

#ifndef USE_INFOBAR
  /* set defaults that can be overridden by rc styles below */
  //gdk_color_parse (DEFAULT_FG_COLOR, &color_fg);
  //gdk_color_parse (DEFAULT_BG_COLOR, &color_bg);
  //xfce_root_warning_set_fg_color (warning, &color_fg);
  //xfce_root_warning_set_bg_color (warning, &color_bg);
  /* get the style properties from any rc files
   * this is supposed to override settings in regular properties */
  gtk_widget_style_get (GTK_WIDGET (warning),
                        "bg-color", &bg_color,
                        "fg-color", &fg_color,
                        "padding", &padding,
                        NULL);

  gdk_color_parse (fg_color ? fg_color : DEFAULT_FG_COLOR, &color_fg);
  gdk_color_parse (bg_color ? bg_color : DEFAULT_BG_COLOR, &color_bg);

  xfce_root_warning_set_fg_color (warning, &color_fg);
  xfce_root_warning_set_bg_color (warning, &color_bg);
  xfce_root_warning_set_padding (warning, padding);

  gtk_event_box_set_above_child (GTK_EVENT_BOX (warning), FALSE);
  gtk_event_box_set_visible_window (GTK_EVENT_BOX (warning), FALSE);
#endif

}


static void
xfce_root_warning_button_clicked (GtkButton       *button,
                                  XfceRootWarning *warning)
{
  g_return_if_fail (XFCE_IS_ROOT_WARNING (warning));
  xfce_root_warning_set_dismissed (warning, TRUE);
}


static void
xfce_root_warning_button_style_set (GtkWidget  *button,
                                    GtkRcStyle *prev_style,
                                    gpointer    user_data)
{
  gint         width;
  gint         height;
  GtkSettings *settings;

  settings = gtk_widget_get_settings (button);
  gtk_icon_size_lookup_for_settings (settings, GTK_ICON_SIZE_MENU, &width, &height);

  /* request the button's size to be 2 pixels larger than the image in it */
  gtk_widget_set_size_request (button, width + 2, height + 2);
}


static void
xfce_root_warning_create_ui (XfceRootWarning *warning)
{
  GtkWidget              *hbox;
  GtkWidget              *image;
  XfceRootWarningPrivate *priv;

  g_return_if_fail (XFCE_IS_ROOT_WARNING (warning));

  priv = G_TYPE_INSTANCE_GET_PRIVATE (warning,
                                      XFCE_TYPE_ROOT_WARNING,
                                      XfceRootWarningPrivate);

  /* remove extra padding/spacing in the dismiss button */
  gtk_rc_parse_string(
    "style \"xfce-gtk-root-warning-button-style\" {\n"
    " GtkWidget::focus-padding = 0\n"
    " GtkWidget::focus-line-width = 0\n"
    " xthickness = 0\n"
    " ythickness = 0\n"
    "}\n"
    "widget \"*.xfce-gtk-root-warning-button\" "
    "style \"xfce-gtk-root-warning-button-style\""
  );

  /* warning icon */
  priv->warning_image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_MENU);
  gtk_widget_set_visible (priv->warning_image, priv->show_icon);

  /* label */
  priv->label = gtk_label_new (DEFAULT_MESSAGE);
  /* ellipsize the string to not force parent widget's minimum size */
  gtk_label_set_ellipsize (GTK_LABEL (priv->label), PANGO_ELLIPSIZE_END);
  gtk_widget_show (priv->label);

  /* dismiss button */
  image = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
  priv->dismiss_button = gtk_button_new ();
  gtk_widget_set_can_focus (priv->dismiss_button, FALSE);
  gtk_widget_set_name (priv->dismiss_button, "xfce-gtk-root-warning-button");
  g_signal_connect (priv->dismiss_button, "style-set",
                    G_CALLBACK (xfce_root_warning_button_style_set), NULL);
  g_signal_connect (priv->dismiss_button, "clicked",
                    G_CALLBACK (xfce_root_warning_button_clicked), warning);
  gtk_container_add (GTK_CONTAINER (priv->dismiss_button), image);
  gtk_widget_show (image);
  gtk_widget_set_visible (GTK_WIDGET (priv->dismiss_button), warning->priv->can_dismiss);

#ifdef USE_INFOBAR

  /* hide the action area in the info bar since it's not used */
  hbox = gtk_info_bar_get_action_area (GTK_INFO_BAR (warning));
  gtk_widget_hide (hbox);

  /* get the hbox (content area) of the info bar */
  hbox = gtk_info_bar_get_content_area (GTK_INFO_BAR (warning));
  gtk_info_bar_set_message_type (GTK_INFO_BAR (warning), GTK_MESSAGE_WARNING);

#else

  /* create a new hbox to and add it inside the main hbox (self) */
  hbox = gtk_hbox_new (FALSE, 6);
  warning->priv->hbox = hbox;
  gtk_container_set_border_width (GTK_CONTAINER (hbox), warning->priv->padding);
  //gtk_box_pack_start (GTK_BOX (warning), hbox, TRUE, TRUE, 0);
  gtk_container_add (GTK_CONTAINER (warning), hbox);
  gtk_widget_show (hbox);

  /* set the text colour of the warning label */
  gtk_widget_modify_fg (GTK_WIDGET (warning->priv->label),
                        GTK_STATE_NORMAL,
                        &warning->priv->fg_color);

#endif

  gtk_box_pack_start (GTK_BOX (hbox), priv->warning_image, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), priv->label, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), priv->dismiss_button, FALSE, TRUE, 0);

  gtk_widget_set_can_focus (GTK_WIDGET (warning), FALSE);
}


/**
 * xfce_root_warning_new:
 *
 * Allocates a new #XfceRootWarning instance.
 *
 * Returns: (transfer full): The newly allocated #XfceRootWarning.
 **/
GtkWidget *
xfce_root_warning_new (void)
{
  return g_object_new(XFCE_TYPE_ROOT_WARNING, NULL);
}


/**
 * xfce_root_warning_new_with_label:
 * @message: (allow none): The initial warning message text.
 *
 * Allocates a new #XfceRootWarning instance with the message property
 * set to @message.
 *
 * Returns: (transfer full): The newly allocated #XfceRootWarning.
 **/
GtkWidget *
xfce_root_warning_new_with_label (const gchar *message)
{
  return g_object_new (XFCE_TYPE_ROOT_WARNING, "message", message, NULL);
}


/**
 * xfce_root_warning_auto_pack:
 * @warning : An #XfceRootWarning widget.
 * @window  : a GtkWindow containing a GtkVBox.
 *
 * Makes an attempt to auto-pack the @warning into the @window's
 * main #GtkVBox.  It's typical for a program to have a #GtkVBox packed
 * into the main #GtkWindow, which might have a GtkMenuBar, a GtkToolbar,
 * and then the rest of the program's widgets.  This function will
 * pack the @warning into the first position in the #GtkVBox after a
 * #GtkMenuBar and/or #GtkToolbar.  If the GtkWindow doesn't hold a
 * #GtkVBox, the @warning won't be packed and the function will
 * return %FALSE.
 *
 * Returns: %TRUE if the @warning was packed, %FALSE otherwise.
 **/
gboolean
xfce_root_warning_auto_pack (XfceRootWarning *warning,
                             GtkWindow       *window)
{
  GtkWidget *vbox;
  GList     *children;
  GList     *iter;
  gint       pos = 0;

  g_return_val_if_fail (GTK_IS_WINDOW (window), FALSE);

  vbox = gtk_bin_get_child (GTK_BIN (window));

  if (GTK_IS_VBOX (vbox))
    {
      children = gtk_container_get_children (GTK_CONTAINER (vbox));

      for (iter= children; iter != NULL; iter = g_list_next (iter))
        {
          if (!GTK_IS_MENU_BAR (iter->data) && !GTK_IS_TOOLBAR (iter->data))
            break;
          else
            pos++;
        }

      gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (warning), FALSE, TRUE, 0);
      gtk_box_reorder_child (GTK_BOX (vbox), GTK_WIDGET (warning), pos);

      g_list_free (children);

      return TRUE;
    }

  return FALSE;
}


/**
 * xfce_root_warning_get_message:
 * @warning : An #XfceRootWarning widget
 *
 * Gets the message text displayed in the widget.
 *
 * Returns: (transfer none): A string containing the current message
 * text. This is an internal string and should not be modified or
 * freed.
 **/
const gchar *
xfce_root_warning_get_message (XfceRootWarning *warning)
{
  g_return_val_if_fail (XFCE_IS_ROOT_WARNING (warning), NULL);
  g_return_val_if_fail (GTK_IS_LABEL (warning->priv->label), NULL);
  return gtk_label_get_text (GTK_LABEL (warning->priv->label));
}


/**
 * xfce_root_warning_set_message:
 * @warning : An #XfceRootWarning widget
 * @message : The text of the warning message to show.
 *
 * Sets the message that will appear in the internal label of the
 * #XfceRootWarning.
 *
 * See also: xfce_root_warning_set_message_markup()
 **/
void
xfce_root_warning_set_message (XfceRootWarning *warning,
                               const gchar     *message)
{
  g_return_if_fail (XFCE_IS_ROOT_WARNING (warning));
  g_return_if_fail (GTK_IS_LABEL (warning->priv->label));
  g_return_if_fail (message != NULL);
  gtk_label_set_text (GTK_LABEL (warning->priv->label), message);
}


/**
 * xfce_root_warning_set_message_markup:
 * @warning        : An #XfceRootWarning widget
 * @message_markup : The warning message text marked-up with GMarkup.
 *
 * Sets the message that will appear in the internal label of the
 * #XfceRootWarning using a marked-up string to allow formatting of the
 * label's text.
 *
 * See also: xfce_root_warning_set_message(), gtk_label_set_markup()
 **/
void
xfce_root_warning_set_message_markup (XfceRootWarning *warning,
                                      const gchar     *message_markup)
{
  g_return_if_fail (XFCE_IS_ROOT_WARNING (warning));
  g_return_if_fail (GTK_IS_LABEL (warning->priv->label));
  g_return_if_fail (message_markup != NULL);
  gtk_label_set_markup (GTK_LABEL (warning->priv->label), message_markup);
}


/**
 * xfce_root_warning_get_activated:
 * @warning: (allow-none): An #XfceRootWarning widget or %NULL.
 *
 * Gets whether the current process is running as root on *nix systems
 * or with elevated priviledges on Windows.  This is the function used
 * to determine whether the root warning needs to be shown or not.
 *
 * On Unix-like systems, this function uses the
 * <function>getuid()</function> function to check if the UID is
 * 0 (root).  On Windows systems, it uses the Win32 API function
 * <function>IsUserAnAdmin()</function>.
 *
 * Returns: %TRUE if the current user/process is running as root
 * or with elevated priviledges, %FALSE if not.
 **/
gboolean
xfce_root_warning_get_activated (XfceRootWarning *warning)
{
  return AM_ROOT;
}


/**
 * xfce_root_warning_get_can_dismiss:
 * @warning: An #XfceRootWarning widget.
 *
 * Gets whether a little close button is shown on the right side of
 * @warning.  This allows the user to dismiss the warning message by
 * clicking the button, which will also cause the
 * #XfceRootWarning::dismissed signal to be emitted.
 *
 * Returns: %TRUE if the dismiss button should be shown, %FALSE otherwise.
 **/
gboolean
xfce_root_warning_get_can_dismiss (XfceRootWarning *warning)
{
  g_return_val_if_fail (XFCE_IS_ROOT_WARNING (warning), FALSE);
  return warning->priv->can_dismiss;
}


/**
 * xfce_root_warning_set_can_dismiss:
 * @warning    : An #XfceRootWarning widget.
 * @can_dismiss: Whether to show the dismiss button or not.
 *
 * Sets whether a little close button is show on the right side of
 * the @warning widget.  When @can_dismiss is %TRUE the button will be
 * shown allowing the user to dismiss the message by clicking it and
 * it will also cause An #XfceRootWarning widget::dismissed signal to be emitted.
 *
 * <note>
 *   <para>
 *     It's probably a good idea to leave this to the default of %FALSE
 *     to make it harder for the user to forget they're using the
 *     root account.
 *   </para>
 * </note>
 **/
void
xfce_root_warning_set_can_dismiss (XfceRootWarning *warning,
                                   gboolean         can_dismiss)
{
  gboolean   changed;

  g_return_if_fail (XFCE_IS_ROOT_WARNING (warning));

  changed = (can_dismiss != warning->priv->can_dismiss);
  warning->priv->can_dismiss = can_dismiss;

  gtk_widget_set_visible (warning->priv->dismiss_button, can_dismiss);

  if (changed)
    g_object_notify (G_OBJECT (warning), "can-dismiss");
}


/**
 * xfce_root_warning_get_dismissed:
 * @warning: An #XfceRootWarning widget.
 *
 * Gets whether the @warning widget was shown and subsequently
 * dismissed, either by the user or by using the
 * xfce_root_warning_set_dismissed() function.
 *
 * If the @warning widget was hasn't been shown yet, this will always
 * return %FALSE.
 *
 * See also:  #XfceRootWarning::dismissed, xfce_root_warning_get_can_dismiss()
 *
 * Returns: %TRUE if the @warning has been dismissed, %FALSE otherwise.
 **/
gboolean
xfce_root_warning_get_dismissed (XfceRootWarning *warning)
{
  g_return_val_if_fail (XFCE_ROOT_WARNING (warning), FALSE);
  return warning->priv->dismissed;
}


/**
 * xfce_root_warning_set_dismissed:
 * @warning  : An #XfceRootWarning widget.
 * @dismissed: Whether to dismiss the @warning or not.
 *
 * Sets whether the @warning is dismissed.  Dismissing a @warning
 * causes the #XfceRootWarning::dismissed signal to be emitted and the
 * @warning widget to be hidden.  Setting @dismissed to %FALSE will
 * cause the @warning to be shown and allow it to be dismissed again.
 *
 * See also: #XfceRootWarning::dismissed, xfce_root_warning_set_can_dismiss()
 */
void
xfce_root_warning_set_dismissed (XfceRootWarning *warning,
                                 gboolean         dismissed)
{
  gboolean changed;

  g_return_if_fail (XFCE_ROOT_WARNING (warning));

  changed = (warning->priv->dismissed != dismissed);
  warning->priv->dismissed = dismissed;

  gtk_widget_set_visible (GTK_WIDGET (warning), !dismissed);

  if (changed)
    g_object_notify (G_OBJECT (warning), "dismissed");

  if (changed && dismissed)
    g_signal_emit_by_name (warning, "dismissed");
}


/**
 * xfce_root_warning_get_show_icon:
 * @warning: An #XfceRootWarning widget.
 *
 * Gets whether the warning icon is shown on the left side of the
 * @warning widget.
 *
 * Returns: %TRUE if the icon is to be shown, %FALSE otherwise.
 **/
gboolean
xfce_root_warning_get_show_icon (XfceRootWarning *warning)
{
  g_return_val_if_fail (XFCE_IS_ROOT_WARNING (warning), FALSE);
  return warning->priv->show_icon;
}


/**
 * xfce_root_warning_set_show_icon:
 * @warning  : An #XfceRootWarning widget.
 * @show_icon: Whether to show the warning icon.
 *
 * Sets whether to show a warning icon (specifically,
 * #GTK_STOCK_DIALOG_WARNING) on the left side of the @warning widget.
 **/
void
xfce_root_warning_set_show_icon (XfceRootWarning *warning,
                                 gboolean         show_icon)
{
  gboolean changed;

  g_return_if_fail (XFCE_IS_ROOT_WARNING(warning));

  changed = (show_icon != warning->priv->show_icon);

  if (changed)
    {
      warning->priv->show_icon = show_icon;
      gtk_widget_set_visible (warning->priv->warning_image, show_icon);
      g_object_notify (G_OBJECT (warning), "show-icon");
    }
}


#ifndef USE_INFOBAR

/**
 * xfce_root_warning_get_fg_color:
 * @warning: An #XfceRootWarning widget.
 *
 * Gets the colour used for the warning message text in the @warning
 * widget.
 *
 * See also: xfce_root_warning_get_bg_color()
 *
 * Returns: (transfer none): A #GdkColor containing the foreground
 * color.  Do not free or modify the returned color.
 **/
const GdkColor *
xfce_root_warning_get_fg_color (XfceRootWarning *warning)
{
  g_return_val_if_fail (XFCE_IS_ROOT_WARNING (warning), NULL);
  return (const GdkColor *) &warning->priv->fg_color;
}


/**
 * xfce_root_warning_set_fg_color:
 * @warning : warning
 * @fg_color: (transfer none): The foreground color for the @warning
 * message text.
 *
 * Sets the colour used for the warning message text in the @warning
 * widget.
 *
 * See also: xfce_root_warning_set_bg_color()
 **/
void
xfce_root_warning_set_fg_color (XfceRootWarning *warning,
                                const GdkColor  *fg_color)
{
  g_return_if_fail (XFCE_IS_ROOT_WARNING (warning));

  if (fg_color && !gdk_color_equal (&warning->priv->fg_color, fg_color))
    {
      memcpy (&warning->priv->fg_color, fg_color, sizeof (GdkColor));
      gtk_widget_modify_fg (warning->priv->label, GTK_STATE_NORMAL, &warning->priv->fg_color);
      g_object_notify (G_OBJECT (warning), "fg-color");
    }
}


/**
 * xfce_root_warning_get_bg_color:
 * @warning: An #XfceRootWarning widget.
 *
 * Gets the background colour of the @warning widget.
 *
 * See also: xfce_root_warning_get_fg_color()
 *
 * Returns: (transfer none): A #GdkColor containing the background
 * colour.  Do not free or modify the returned color.
 **/
const GdkColor *
xfce_root_warning_get_bg_color (XfceRootWarning *warning)
{
  g_return_val_if_fail (XFCE_IS_ROOT_WARNING (warning), NULL);
  return &warning->priv->bg_color;
}


/**
 * xfce_root_warning_set_bg_color:
 * @warning : warning
 * @bg_color: (transfer none): The background colour of the @warning widget.
 *
 * Sets the background colour of the @warning widget. The colour is
 * drawn below the highlighting gradient, if it's enabled.
 *
 * See also: xfce_root_warning_set_fg_color()
 **/
void
xfce_root_warning_set_bg_color (XfceRootWarning *warning,
                                const GdkColor  *bg_color)
{
  g_return_if_fail (XFCE_IS_ROOT_WARNING (warning));

  if (bg_color && !gdk_color_equal (&warning->priv->bg_color, bg_color))
    {

      memcpy (&warning->priv->bg_color, bg_color, sizeof (GdkColor));

      /* stored so we don't need to compute it on each expose-event */
      warning->priv->cairo_bg_color.red   = (gdouble) ((gdouble) bg_color->red / (gdouble) G_MAXUINT16);
      warning->priv->cairo_bg_color.green = (gdouble) ((gdouble) bg_color->green / (gdouble) G_MAXUINT16);
      warning->priv->cairo_bg_color.blue  = (gdouble) ((gdouble) bg_color->blue / (gdouble) G_MAXUINT16);

      /* re-draw with the new background colour */
      gtk_widget_queue_draw (GTK_WIDGET (warning));

      g_object_notify (G_OBJECT (warning), "bg-color");
    }
}


/**
 * xfce_root_warning_get_highlight:
 * @warning: An #XfceRootWarning widget.
 *
 * Gets whether or not a highlighting effect is drawn over the
 * background colour.
 *
 * Returns: %TRUE if the highlighting effect is active, %FALSE otherwise.
 **/
gboolean
xfce_root_warning_get_highlight (XfceRootWarning *warning)
{
  g_return_val_if_fail (XFCE_IS_ROOT_WARNING (warning), FALSE);
  return warning->priv->highlight;
}


/**
 * xfce_root_warning_set_highlight:
 * @warning  : An #XfceRootWarning widget.
 * @highlight: Whether ot not use the highlighting effect.
 *
 * Sets whether or not a highlighting effect is drawn over the
 * background colour.  If @highlighting is %TRUE, during the @warning
 * widget's expose event, a gradient will be drawn over the background
 * colour giving the widget a bit of depth.
 **/
void
xfce_root_warning_set_highlight (XfceRootWarning *warning,
                                 gboolean         highlight)
{
  gboolean changed;

  g_return_if_fail (XFCE_IS_ROOT_WARNING (warning));

  changed = (highlight != warning->priv->highlight);

  if (changed)
    {
      warning->priv->highlight = highlight;

      /* re-draw with the new highlight setting */
      gtk_widget_queue_draw (GTK_WIDGET (warning));

      g_object_notify (G_OBJECT (warning), "highlight");
    }
}


/**
 * xfce_root_warning_get_padding:
 * @warning: An #XfceRootWarning widget.
 *
 * Gets the amount of padding, in pixels, between the outer edge of the
 * @warning widget and the outer edge of the internal #GtkHBox.
 *
 * Returns: The number of padding in pixels, from 0 to 100.
 **/
guint
xfce_root_warning_get_padding (XfceRootWarning *warning)
{
  g_return_val_if_fail (XFCE_IS_ROOT_WARNING (warning), 0);
  return warning->priv->padding;
}


/**
 * xfce_root_warning_set_padding:
 * @warning: An #XfceRootWarning widget.
 * @padding: The padding inside the @warning widget, in pixels.
 *
 * Sets the amount of padding, in pixels, between the outer edge of the
 * @warning widget and the outer edge of the internal #GtkHBox.  This
 * actually sets the #GtkContainer:border-width property on the internal
 * #GtkHBox.
 **/
void
xfce_root_warning_set_padding (XfceRootWarning *warning,
                               guint            padding)
{
  gboolean changed;

  g_return_if_fail (XFCE_IS_ROOT_WARNING (warning));

  changed = (padding != warning->priv->padding);

  if (changed)
    {
      warning->priv->padding = padding;
      gtk_container_set_border_width (GTK_CONTAINER (warning->priv->hbox), padding);
      g_object_notify (G_OBJECT (warning), "padding");
    }
}

#endif /* USE_INFOBAR */

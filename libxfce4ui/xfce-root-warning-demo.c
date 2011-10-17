#include <gtk/gtk.h>
#include <libxfce4ui/xfce-root-warning.h>


static void
root_warning_dismissed (XfceRootWarning *warning,
                        gpointer         user_data)
{
  g_debug ("Root warning dismissed!");
}

static GtkWidget *build_widgets (XfceRootWarning *warning);


int main (int   argc,
          char *argv[])
{
  GtkWidget *win;
  GtkWidget *vbox;
  GtkWidget *table;
  GtkWidget *warning;

  gtk_init (&argc, &argv);

  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_default_size (GTK_WINDOW (win), 640, -1);
  gtk_window_set_title (GTK_WINDOW (win), "XfceRootWarning Test");
  g_signal_connect (win, "destroy", gtk_main_quit, NULL);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (win), vbox);
  gtk_widget_show (vbox);

  warning = xfce_root_warning_new ();

  g_signal_connect (warning, "dismissed", G_CALLBACK (root_warning_dismissed), NULL);

  //xfce_root_warning_auto_pack (XFCE_ROOT_WARNING (warning), GTK_WINDOW (win));
  gtk_box_pack_start (GTK_BOX (vbox), warning, FALSE, TRUE, 0);

  gtk_widget_show (warning);

  table = build_widgets (XFCE_ROOT_WARNING (warning));
  gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, TRUE, 0);

  gtk_widget_show (win);
  gtk_main ();

  return 0;
}


static void
on_can_dismiss_checked (GtkToggleButton *button,
                        XfceRootWarning *warning)
{
  gboolean active;
  active = gtk_toggle_button_get_active (button);
  xfce_root_warning_set_can_dismiss (warning, active);
}


static void
on_dismiss_checked (GtkToggleButton *button,
                    XfceRootWarning *warning)
{
  gboolean active;
  active = gtk_toggle_button_get_active (button);
  xfce_root_warning_set_dismissed (warning, active);
}

static void
on_dismissed (XfceRootWarning *warning,
              GtkToggleButton *button)
{
  gtk_toggle_button_set_active (button, xfce_root_warning_get_dismissed (warning));
}

static void
on_show_icon_checked (GtkToggleButton *button,
                      XfceRootWarning *warning)
{
  gboolean active;
  active = gtk_toggle_button_get_active (button);
  xfce_root_warning_set_show_icon (warning, active);
}


static void
on_use_markup_checked (GtkToggleButton *button,
                       GtkEntry        *entry)
{
  gboolean active;
  active = gtk_toggle_button_get_active (button);
  g_object_set_data (G_OBJECT (entry), "use_markup", GINT_TO_POINTER (active));
  g_object_notify (G_OBJECT (entry), "text");
}


static void
on_message_text_notify (GObject         *object,
                        GParamSpec      *pspec,
                        XfceRootWarning *warning)
{
  gboolean  use_markup;
  gchar    *text;
  g_object_get (G_OBJECT (object), "text", &text, NULL);
  use_markup = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (object), "use_markup"));
  if (use_markup)
    xfce_root_warning_set_message_markup (warning, text);
  else
    xfce_root_warning_set_message (warning, text);
  g_free (text);
}


static void
on_fg_color_set (GtkColorButton  *button,
                 XfceRootWarning *warning)
{
  GdkColor color;
  gtk_color_button_get_color (button, &color);
  xfce_root_warning_set_fg_color (warning, &color);
}


static void
on_bg_color_set (GtkColorButton  *button,
                 XfceRootWarning *warning)
{
  GdkColor color;
  gtk_color_button_get_color (button, &color);
  xfce_root_warning_set_bg_color (warning, &color);
}


static void
on_padding_value_changed (GtkSpinButton   *button,
                          XfceRootWarning *warning)
{
  gint value;
  value = gtk_spin_button_get_value_as_int (button);
  xfce_root_warning_set_padding (warning, (guint) value);
}


static GtkWidget *
build_widgets (XfceRootWarning *warning)
{
  gint           row = 0;
  gint           rows = 6;
  GtkWidget     *table;
  GtkWidget     *label;
  GtkWidget     *check;
  GtkWidget     *hbox;
  GtkWidget     *ent;
  GtkWidget     *clr;
  GtkWidget     *spin;
  GtkAdjustment *adjust;

  table = gtk_table_new (rows, 2, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table), 6);
  gtk_table_set_row_spacings (GTK_TABLE (table), 6);
  gtk_container_set_border_width (GTK_CONTAINER (table), 12);

  label = gtk_label_new ("Can Dismiss?");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);

  check = gtk_check_button_new ();
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check),
                                xfce_root_warning_get_can_dismiss (warning));
  g_signal_connect (check, "toggled", G_CALLBACK (on_can_dismiss_checked), warning);
  gtk_table_attach_defaults (GTK_TABLE (table), check, 1, 2, row, row + 1);

  row++;

  label = gtk_label_new ("Dismissed?");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);

  check = gtk_check_button_new ();
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check),
                                xfce_root_warning_get_dismissed (warning));
  g_signal_connect (check, "toggled", G_CALLBACK (on_dismiss_checked), warning);
  g_signal_connect (warning, "dismissed", G_CALLBACK (on_dismissed), GTK_TOGGLE_BUTTON (check));
  gtk_table_attach_defaults (GTK_TABLE (table), check, 1, 2, row, row + 1);

  row++;

  label = gtk_label_new ("Show Icon?");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);

  check = gtk_check_button_new ();
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check),
                                xfce_root_warning_get_show_icon (warning));
  g_signal_connect (check, "toggled", G_CALLBACK (on_show_icon_checked), warning);
  gtk_table_attach_defaults (GTK_TABLE (table), check, 1, 2, row, row + 1);

  row++;

  label = gtk_label_new ("Message:");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);

  hbox = gtk_hbox_new (FALSE, 6);
  gtk_table_attach_defaults (GTK_TABLE (table), hbox, 1, 2, row, row + 1);

  ent = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (ent), xfce_root_warning_get_message (warning));
  g_object_set_data (G_OBJECT (ent), "use_markup", GINT_TO_POINTER (0));
  g_signal_connect (ent, "notify::text", G_CALLBACK (on_message_text_notify), warning);
  gtk_box_pack_start (GTK_BOX (hbox), ent, TRUE, TRUE, 0);

  check = gtk_check_button_new_with_label ("Use markup");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), FALSE);
  g_signal_connect (check, "toggled", G_CALLBACK (on_use_markup_checked), ent);
  gtk_box_pack_start (GTK_BOX (hbox), check, FALSE, TRUE, 0);

  row++;

  label = gtk_label_new ("Padding:");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);

  adjust = GTK_ADJUSTMENT (gtk_adjustment_new (6.0, 0.0, 100.0, 1.0, 0.0, 0.0));
  spin = gtk_spin_button_new (adjust, 1, 0);
  g_signal_connect (spin, "value-changed", G_CALLBACK (on_padding_value_changed), warning);
  gtk_table_attach (GTK_TABLE (table), spin, 1, 2, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);

  row ++;

  label = gtk_label_new ("FG Color:");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);

  clr = gtk_color_button_new_with_color (xfce_root_warning_get_fg_color (warning));
  g_signal_connect (clr, "color-set", G_CALLBACK (on_fg_color_set), warning);
  gtk_table_attach_defaults (GTK_TABLE (table), clr, 1, 2, row, row + 1);

  row++;

  label = gtk_label_new ("BG Color:");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);

  clr = gtk_color_button_new_with_color (xfce_root_warning_get_bg_color (warning));
  g_signal_connect (clr, "color-set", G_CALLBACK (on_bg_color_set), warning);
  gtk_table_attach_defaults (GTK_TABLE (table), clr, 1, 2, row, row + 1);

  row++;

  gtk_widget_show_all (table);

  return table;
}

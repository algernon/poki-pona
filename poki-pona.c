/* poki-pona -- algernon's terminal emulator
 * Copyright (C) 2019  Gergely Nagy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vte/vte.h>
#include <gtk/gtk.h>

#define CLR_R(x)   (((x) & 0xff0000) >> 16)
#define CLR_G(x)   (((x) & 0x00ff00) >>  8)
#define CLR_B(x)   (((x) & 0x0000ff) >>  0)
#define CLR_16(x)  ((double)(x) / 0xff)
#define CLR_GDK(x) (const GdkRGBA){ .red = CLR_16(CLR_R(x)),  \
      .green = CLR_16(CLR_G(x)),                              \
      .blue = CLR_16(CLR_B(x)),                               \
      .alpha = 0 }

static GtkApplication *app;

static void
quit(void) {
  g_application_quit(G_APPLICATION(app));
}

static GtkWidget *
window_create(GtkWidget *terminal) {
  GtkWidget *window;

  window = gtk_application_window_new(GTK_APPLICATION(app));
  gtk_window_set_title(GTK_WINDOW(window), "poki pona");
  gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
  gtk_window_set_icon_name(GTK_WINDOW(window), "utilities-terminal-symbolic");

  /* Connect some signals */
  g_signal_connect(window, "delete-event", G_CALLBACK(quit), NULL);

  /* Put widgets together and run the main loop */
  gtk_container_add(GTK_CONTAINER(window), terminal);
  gtk_window_maximize(GTK_WINDOW(window));

  return window;
}

static GtkWidget *
terminal_create(const char *font) {
  GtkWidget *terminal;
  GtkBindingSet *bindings;
  PangoFontDescription *pfd;

  terminal = vte_terminal_new();

  /* Set up the colors */
  vte_terminal_set_colors
    (
     VTE_TERMINAL(terminal),
     &CLR_GDK(0x334446),
     NULL,
     (const GdkRGBA[])
     {
      CLR_GDK(0xfdf6e3),
      CLR_GDK(0xdc322f),
      CLR_GDK(0x859900),
      CLR_GDK(0xb58900),
      CLR_GDK(0x268bd2),
      CLR_GDK(0x6c71c4),
      CLR_GDK(0x2aa198),
      CLR_GDK(0x657b83),

      CLR_GDK(0x586e75),
      CLR_GDK(0xdc322f),
      CLR_GDK(0x859900),
      CLR_GDK(0xb58900),
      CLR_GDK(0x268bd2),
      CLR_GDK(0x6c71c4),
      CLR_GDK(0x2aa198),
      CLR_GDK(0x657b83)
     }, 16);

  /* Set up the font */
  pfd = pango_font_description_from_string(font);
  vte_terminal_set_font(VTE_TERMINAL(terminal), pfd);

  /* Start a new shell */
  gchar **envp = g_get_environ();
  gchar **command = (gchar *[]){g_strdup(g_environ_getenv(envp, "SHELL")), NULL };
  g_strfreev(envp);

  vte_terminal_spawn_async
    (
     VTE_TERMINAL(terminal),
     VTE_PTY_DEFAULT,
     NULL,       /* working directory  */
     command,    /* command */
     NULL,       /* environment */
     0,          /* spawn flags */
     NULL, NULL, /* child setup */
     NULL,       /* child pid */
     -1, NULL, NULL, NULL
    );

  g_signal_connect(terminal, "child-exited", G_CALLBACK(quit), NULL);

  /* Set up some terminal properties */
  vte_terminal_set_scroll_on_output(VTE_TERMINAL(terminal), FALSE);
  vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(terminal), TRUE);
  vte_terminal_set_rewrap_on_resize(VTE_TERMINAL(terminal), TRUE);
  vte_terminal_set_mouse_autohide(VTE_TERMINAL(terminal), TRUE);
  vte_terminal_set_allow_hyperlink(VTE_TERMINAL(terminal), TRUE);

  /* Set up custom key bindings */
  bindings = gtk_binding_set_by_class(VTE_TERMINAL_GET_CLASS(terminal));
  gtk_binding_entry_add_signal(bindings, GDK_KEY_c, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                               "copy-clipboard", 0);
  gtk_binding_entry_add_signal(bindings, GDK_KEY_v, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                               "paste-clipboard", 0);

  return terminal;
}

static void
activate() {
  GtkWidget *window;
  GList *list;

  list = gtk_application_get_windows(app);

  if (list) {
    gtk_window_present(GTK_WINDOW(list->data));
    return;
  }

  window = window_create(terminal_create("Monospace Regular 13"));
  gtk_widget_show_all(window);
}

int
main(int argc, char *argv[])
{
  int status;

  app = gtk_application_new("org.madhouse.poki-pona", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}

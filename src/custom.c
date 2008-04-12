/*	$calcurse: custom.c,v 1.20 2008/04/12 21:14:03 culot Exp $	*/

/*
 * Calcurse - text-based organizer
 * Copyright (c) 2004-2008 Frederic Culot
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Send your feedback or comments to : calcurse@culot.org
 * Calcurse home page : http://culot.org/calcurse
 *
 */

#include <string.h>
#include <stdlib.h>

#include "custom.h"
#include "i18n.h"
#include "io.h"
#include "utils.h"
#include "apoint.h"

static struct attribute_s attr;

static bool
fill_config_var (char *string)
{
  if (strncmp (string, "yes", 3) == 0)
    return (true);
  else if (strncmp (string, "no", 2) == 0)
    return (false);
  else
    {
      fputs (_("FATAL ERROR in fill_config_var: "
	       "wrong configuration variable format.\n"), stderr);
      return (EXIT_FAILURE);
    }
}

/* 
 * Load user color theme from file. 
 * Need to handle calcurse versions prior to 1.8, where colors where handled
 * differently (number between 1 and 8).
 */
static void
custom_load_color (char *color, int background)
{
#define AWAITED_COLORS	2

  int i, len, color_num;
  char c[AWAITED_COLORS][BUFSIZ];
  int colr[AWAITED_COLORS];
  const char *wrong_color_number =
      _("FATAL ERROR in custom_load_color: wrong color number.\n");
  const char *wrong_color_name =
      _("FATAL ERROR in custom_load_color: wrong color name.\n");
  const char *wrong_variable_format =
      _("FATAL ERROR in custom_load_color: "
	"wrong configuration variable format.\n");

  len = strlen (color);

  if (len > 1)
    {
      /* New version configuration */
      if (sscanf (color, "%s on %s", c[0], c[1]) != AWAITED_COLORS)
	{
	  fputs (_("FATAL ERROR in custom_load_color: "
		   "missing colors in config file.\n"), stderr);
	  exit (EXIT_FAILURE);
	  /* NOTREACHED */
	};

      for (i = 0; i < AWAITED_COLORS; i++)
	{
	  if (!strncmp (c[i], "black", 5))
	    colr[i] = COLOR_BLACK;
	  else if (!strncmp (c[i], "red", 3))
	    colr[i] = COLOR_RED;
	  else if (!strncmp (c[i], "green", 5))
	    colr[i] = COLOR_GREEN;
	  else if (!strncmp (c[i], "yellow", 6))
	    colr[i] = COLOR_YELLOW;
	  else if (!strncmp (c[i], "blue", 4))
	    colr[i] = COLOR_BLUE;
	  else if (!strncmp (c[i], "magenta", 7))
	    colr[i] = COLOR_MAGENTA;
	  else if (!strncmp (c[i], "cyan", 4))
	    colr[i] = COLOR_CYAN;
	  else if (!strncmp (c[i], "white", 5))
	    colr[i] = COLOR_WHITE;
	  else if (!strncmp (c[i], "default", 7))
	    colr[i] = background;
	  else
	    {
	      fputs (wrong_color_name, stderr);
	      exit (EXIT_FAILURE);
	      /* NOTREACHED */
	    }
	}
      init_pair (COLR_CUSTOM, colr[0], colr[1]);
    }
  else if (len > 0 && len < 2)
    {
      /* Old version configuration */
      color_num = atoi (color);

      switch (color_num)
	{
	case 0:
	  colorize = false;
	  break;
	case 1:
	  init_pair (COLR_CUSTOM, COLOR_RED, background);
	  break;
	case 2:
	  init_pair (COLR_CUSTOM, COLOR_GREEN, background);
	  break;
	case 3:
	  init_pair (COLR_CUSTOM, COLOR_BLUE, background);
	  break;
	case 4:
	  init_pair (COLR_CUSTOM, COLOR_CYAN, background);
	  break;
	case 5:
	  init_pair (COLR_CUSTOM, COLOR_YELLOW, background);
	  break;
	case 6:
	  init_pair (COLR_CUSTOM, COLOR_BLACK, COLR_GREEN);
	  break;
	case 7:
	  init_pair (COLR_CUSTOM, COLOR_BLACK, COLR_YELLOW);
	  break;
	case 8:
	  init_pair (COLR_CUSTOM, COLOR_RED, COLR_BLUE);
	  break;
	default:
	  fputs (wrong_color_number, stderr);
	  exit (EXIT_FAILURE);
	  /* NOTREACHED */
	}
    }
  else
    {
      fputs (wrong_variable_format, stderr);
      exit (EXIT_FAILURE);
      /* NOTREACHED */
    }
}

/* 
 * Define window attributes (for both color and non-color terminals):
 * ATTR_HIGHEST are for window titles
 * ATTR_HIGH are for month and days names
 * ATTR_MIDDLE are for the selected day inside calendar panel
 * ATTR_LOW are for days inside calendar panel which contains an event
 * ATTR_LOWEST are for current day inside calendar panel
 */
void
custom_init_attr (void)
{
  attr.color[ATTR_HIGHEST] = COLOR_PAIR (COLR_CUSTOM);
  attr.color[ATTR_HIGH] = COLOR_PAIR (COLR_HIGH);
  attr.color[ATTR_MIDDLE] = COLOR_PAIR (COLR_RED);
  attr.color[ATTR_LOW] = COLOR_PAIR (COLR_CYAN);
  attr.color[ATTR_LOWEST] = COLOR_PAIR (COLR_YELLOW);
  attr.color[ATTR_TRUE] = COLOR_PAIR (COLR_GREEN);
  attr.color[ATTR_FALSE] = COLOR_PAIR (COLR_RED);

  attr.nocolor[ATTR_HIGHEST] = A_BOLD;
  attr.nocolor[ATTR_HIGH] = A_REVERSE;
  attr.nocolor[ATTR_MIDDLE] = A_REVERSE;
  attr.nocolor[ATTR_LOW] = A_UNDERLINE;
  attr.nocolor[ATTR_LOWEST] = A_BOLD;
  attr.nocolor[ATTR_TRUE] = A_BOLD;
  attr.nocolor[ATTR_FALSE] = A_DIM;
}

/* Apply window attribute */
void
custom_apply_attr (WINDOW * win, int attr_num)
{
  if (colorize)
    wattron (win, attr.color[attr_num]);
  else
    wattron (win, attr.nocolor[attr_num]);
}

/* Remove window attribute */
void
custom_remove_attr (WINDOW * win, int attr_num)
{
  if (colorize)
    wattroff (win, attr.color[attr_num]);
  else
    wattroff (win, attr.nocolor[attr_num]);
}

/* Load the user configuration. */
void
custom_load_conf (conf_t * conf, int background)
{
  FILE *data_file;
  char *mesg_line1 = _("Failed to open config file");
  char *mesg_line2 = _("Press [ENTER] to continue");
  char buf[100], e_conf[100];
  int var;

  data_file = fopen (path_conf, "r");
  if (data_file == NULL)
    {
      status_mesg (mesg_line1, mesg_line2);
      wnoutrefresh (win[STA].p);
      doupdate ();
      wgetch (win[STA].p);
    }
  var = 0;
  pthread_mutex_lock (&nbar->mutex);
  for (;;)
    {
      if (fgets (buf, 99, data_file) == NULL)
	{
	  break;
	}
      io_extract_data (e_conf, buf, strlen (buf));

      switch (var)
	{
	case CUSTOM_CONF_NOVARIABLE:
	  break;
	case CUSTOM_CONF_AUTOSAVE:
	  conf->auto_save = fill_config_var (e_conf);
	  var = 0;
	  break;
	case CUSTOM_CONF_CONFIRMQUIT:
	  conf->confirm_quit = fill_config_var (e_conf);
	  var = 0;
	  break;
	case CUSTOM_CONF_CONFIRMDELETE:
	  conf->confirm_delete = fill_config_var (e_conf);
	  var = 0;
	  break;
	case CUSTOM_CONF_SKIPSYSTEMDIALOGS:
	  conf->skip_system_dialogs = fill_config_var (e_conf);
	  var = 0;
	  break;
	case CUSTOM_CONF_SKIPPROGRESSBAR:
	  conf->skip_progress_bar = fill_config_var (e_conf);
	  var = 0;
	  break;
	case CUSTOM_CONF_WEEKBEGINSONMONDAY:
	  if (fill_config_var (e_conf))
	    calendar_set_first_day_of_week (MONDAY);
	  else
	    calendar_set_first_day_of_week (SUNDAY);
	  var = 0;
	  break;
	case CUSTOM_CONF_COLORTHEME:
	  custom_load_color (e_conf, background);
	  var = 0;
	  break;
	case CUSTOM_CONF_LAYOUT:
	  wins_set_layout (atoi (e_conf));
	  var = 0;
	  break;
	case CUSTOM_CONF_NOTIFYBARSHOW:
	  nbar->show = fill_config_var (e_conf);
	  var = 0;
	  break;
	case CUSTOM_CONF_NOTIFYBARDATE:
	  strncpy (nbar->datefmt, e_conf, strlen (e_conf) + 1);
	  var = 0;
	  break;
	case CUSTOM_CONF_NOTIFYBARCLOCK:
	  strncpy (nbar->timefmt, e_conf, strlen (e_conf) + 1);
	  var = 0;
	  break;
	case CUSTOM_CONF_NOTIFYBARWARNING:
	  nbar->cntdwn = atoi (e_conf);
	  var = 0;
	  break;
	case CUSTOM_CONF_NOTIFYBARCOMMAND:
	  strncpy (nbar->cmd, e_conf, strlen (e_conf) + 1);
	  var = 0;
	  break;
	case CUSTOM_CONF_OUTPUTDATEFMT:
	  if (e_conf[0] != '\0')
	    strncpy (conf->output_datefmt, e_conf, strlen (e_conf) + 1);
	  var = 0;
	  break;
	case CUSTOM_CONF_INPUTDATEFMT:
	  conf->input_datefmt = atoi (e_conf);
	  if (conf->input_datefmt < 1 || conf->input_datefmt > 3)
	    conf->input_datefmt = 1;
	  var = 0;
	  break;
	default:
	  fputs (_("FATAL ERROR in custom_load_conf: "
		   "configuration variable unknown.\n"), stderr);
	  exit (EXIT_FAILURE);
	  /* NOTREACHED */
	}

      if (strncmp (e_conf, "auto_save=", 10) == 0)
	var = CUSTOM_CONF_AUTOSAVE;
      else if (strncmp (e_conf, "confirm_quit=", 13) == 0)
	var = CUSTOM_CONF_CONFIRMQUIT;
      else if (strncmp (e_conf, "confirm_delete=", 15) == 0)
	var = CUSTOM_CONF_CONFIRMDELETE;
      else if (strncmp (e_conf, "skip_system_dialogs=", 20) == 0)
	var = CUSTOM_CONF_SKIPSYSTEMDIALOGS;
      else if (strncmp (e_conf, "skip_progress_bar=", 18) == 0)
	var = CUSTOM_CONF_SKIPPROGRESSBAR;
      else if (strncmp (e_conf, "week_begins_on_monday=", 23) == 0)
	var = CUSTOM_CONF_WEEKBEGINSONMONDAY;
      else if (strncmp (e_conf, "color-theme=", 12) == 0)
	var = CUSTOM_CONF_COLORTHEME;
      else if (strncmp (e_conf, "layout=", 7) == 0)
	var = CUSTOM_CONF_LAYOUT;
      else if (strncmp (e_conf, "notify-bar_show=", 16) == 0)
	var = CUSTOM_CONF_NOTIFYBARSHOW;
      else if (strncmp (e_conf, "notify-bar_date=", 16) == 0)
	var = CUSTOM_CONF_NOTIFYBARDATE;
      else if (strncmp (e_conf, "notify-bar_clock=", 17) == 0)
	var = CUSTOM_CONF_NOTIFYBARCLOCK;
      else if (strncmp (e_conf, "notify-bar_warning=", 19) == 0)
	var = CUSTOM_CONF_NOTIFYBARWARNING;
      else if (strncmp (e_conf, "notify-bar_command=", 19) == 0)
	var = CUSTOM_CONF_NOTIFYBARCOMMAND;
      else if (strncmp (e_conf, "output_datefmt=", 12) == 0)
	var = CUSTOM_CONF_OUTPUTDATEFMT;
      else if (strncmp (e_conf, "input_datefmt=", 12) == 0)
	var = CUSTOM_CONF_INPUTDATEFMT;
    }
  fclose (data_file);
  pthread_mutex_unlock (&nbar->mutex);
}

/* Draws the configuration bar */
void
config_bar (void)
{
  int smlspc, spc;

  smlspc = 2;
  spc = 15;

  custom_apply_attr (win[STA].p, ATTR_HIGHEST);
  mvwprintw (win[STA].p, 0, 2, "Q");
  mvwprintw (win[STA].p, 1, 2, "G");
  mvwprintw (win[STA].p, 0, 2 + spc, "L");
  mvwprintw (win[STA].p, 1, 2 + spc, "C");
  mvwprintw (win[STA].p, 0, 2 + 2 * spc, "N");
  custom_remove_attr (win[STA].p, ATTR_HIGHEST);

  mvwprintw (win[STA].p, 0, 2 + smlspc, _("Exit"));
  mvwprintw (win[STA].p, 1, 2 + smlspc, _("General"));
  mvwprintw (win[STA].p, 0, 2 + spc + smlspc, _("Layout"));
  mvwprintw (win[STA].p, 1, 2 + spc + smlspc, _("Color"));
  mvwprintw (win[STA].p, 0, 2 + 2 * spc + smlspc, _("Notify"));

  wnoutrefresh (win[STA].p);
  wmove (win[STA].p, 0, 0);
  doupdate ();
}

/* Choose the layout */
void
layout_config (void)
{
  int ch;
  char *layout_mesg =
    _("Pick the desired layout on next screen [press ENTER]");
  char *choice_mesg =
    _("('A'= Appointment panel, 'C'= calendar panel, 'T'= todo panel)");
  char *layout_up_mesg =
    _("    AC       AT       CA       TA       TC       TA       CT       AT");
  char *layout_down_mesg =
    _(" [1]AT    [2]AC    [3]TA    [4]CA    [5]TA    [6]TC    [7]AT    [8]CT");

  status_mesg (layout_mesg, choice_mesg);
  wgetch (win[STA].p);
  status_mesg (layout_up_mesg, layout_down_mesg);
  wnoutrefresh (win[STA].p);
  doupdate ();
  while ((ch = wgetch (win[STA].p)) != 'q')
    {
      if (ch <= '8' && ch >= '1')
	{
	  wins_set_layout (ch - '0');
	  return;
	}
    }
}

/* 
 * Create a configuration window and initialize status and notification bar 
 * (useful in case of window resize).
 */
void
custom_confwin_init (window_t *confwin, char *label)
{
  wins_get_config ();
  confwin->h = (notify_bar ())? row - 3 : row - 2;
  confwin->p = newwin (confwin->h, col, 0, 0);
  box (confwin->p, 0, 0);
  wins_show (confwin->p, label);
  delwin (win[STA].p);
  win[STA].p = newwin (win[STA].h, win[STA].w, win[STA].y, win[STA].x);
  keypad (win[STA].p, TRUE);
  if (notify_bar ())
    {
      notify_reinit_bar (win[NOT].h, win[NOT].w, win[NOT].y, win[NOT].x);
      notify_update_bar ();
    }
}

/* 
 * Used to display available colors in color configuration menu.
 * This is useful for window resizing.
 */
static void
display_color_config (window_t *cwin, int *mark_fore, int *mark_back,
		      int cursor, int need_reset, int theme_changed)
{
#define	SIZE 			(2 * (NBUSERCOLORS + 1))
#define DEFAULTCOLOR		255
#define DEFAULTCOLOR_EXT	-1
#define CURSOR			(32 | A_REVERSE)
#define SPACE			32
#define MARK			88

  char *fore_txt = _("Foreground");
  char *back_txt = _("Background");
  char *default_txt = _("(terminal's default)");
  char *bar = "          ";
  char *box = "[ ]";
  char *choose_color_1 = _("Use 'X' or SPACE to select a color, "
			   "'H/L' 'J/K' or arrow keys to move");
  char *choose_color_2 = _("('0' for no color, 'Q' to exit) :");
  char label[BUFSIZ];
  const unsigned Y = 3;
  const unsigned XOFST = 5;
  const unsigned YSPC = (row - 8) / (NBUSERCOLORS + 1);
  const unsigned BARSIZ = strlen (bar);
  const unsigned BOXSIZ = strlen (box);
  const unsigned XSPC = (col - 2 * BARSIZ - 2 * BOXSIZ - 6) / 3;
  const unsigned XFORE = XSPC;
  const unsigned XBACK = 2 * XSPC + BOXSIZ + XOFST + BARSIZ;
  enum
  { YPOS, XPOS, NBPOS };
  unsigned i;
  int pos[SIZE][NBPOS];
  short colr_fore, colr_back;
  int colr[SIZE] = {
    COLR_RED, COLR_GREEN, COLR_YELLOW, COLR_BLUE,
    COLR_MAGENTA, COLR_CYAN, COLR_DEFAULT,
    COLR_RED, COLR_GREEN, COLR_YELLOW, COLR_BLUE,
    COLR_MAGENTA, COLR_CYAN, COLR_DEFAULT
  };

  for (i = 0; i < NBUSERCOLORS + 1; i++)
    {
      pos[i][YPOS] = Y + YSPC * (i + 1);
      pos[NBUSERCOLORS + i + 1][YPOS] = Y + YSPC * (i + 1);
      pos[i][XPOS] = XFORE;
      pos[NBUSERCOLORS + i + 1][XPOS] = XBACK;
    }

  if (need_reset)
    {
      if (cwin->p != NULL)
	delwin (cwin->p);
      snprintf (label, BUFSIZ, _("CalCurse %s | color theme"), VERSION);
      custom_confwin_init (cwin, label);
    }

  if (colorize)
    {
      if (theme_changed)
	{
	  pair_content (colr[*mark_fore], &colr_fore, 0L);
	  if (colr_fore == 255)
	    colr_fore = -1;
	  pair_content (colr[*mark_back], &colr_back, 0L);
	  if (colr_back == 255)
	    colr_back = -1;
	  init_pair (COLR_CUSTOM, colr_fore, colr_back);
	}
      else
	{
	  /* Retrieve the actual color theme. */
	  pair_content (COLR_CUSTOM, &colr_fore, &colr_back);

	  if ((colr_fore == DEFAULTCOLOR) || (colr_fore == DEFAULTCOLOR_EXT))
	    *mark_fore = NBUSERCOLORS;
	  else
	    for (i = 0; i < NBUSERCOLORS + 1; i++)
	      if (colr_fore == colr[i])
		*mark_fore = i;

	  if ((colr_back == DEFAULTCOLOR) || (colr_back == DEFAULTCOLOR_EXT))
	    *mark_back = SIZE - 1;
	  else
	    for (i = 0; i < NBUSERCOLORS + 1; i++)
	      if (colr_back == colr[NBUSERCOLORS + 1 + i])
		*mark_back = NBUSERCOLORS + 1 + i;
	}
    }

  /* color boxes */
  for (i = 0; i < SIZE - 1; i++)
    {
      mvwprintw (cwin->p, pos[i][YPOS], pos[i][XPOS], box);
      wattron (cwin->p, COLOR_PAIR (colr[i]) | A_REVERSE);
      mvwprintw (cwin->p, pos[i][YPOS], pos[i][XPOS] + XOFST, bar);
      wattroff (cwin->p, COLOR_PAIR (colr[i]) | A_REVERSE);
    }

  /* Terminal's default color */
  i = SIZE - 1;
  mvwprintw (cwin->p, pos[i][YPOS], pos[i][XPOS], box);
  wattron (cwin->p, COLOR_PAIR (colr[i]));
  mvwprintw (cwin->p, pos[i][YPOS], pos[i][XPOS] + XOFST, bar);
  wattroff (cwin->p, COLOR_PAIR (colr[i]));
  mvwprintw (cwin->p, pos[NBUSERCOLORS][YPOS] + 1,
	     pos[NBUSERCOLORS][XPOS] + XOFST, default_txt);
  mvwprintw (cwin->p, pos[SIZE - 1][YPOS] + 1,
	     pos[SIZE - 1][XPOS] + XOFST, default_txt);

  custom_apply_attr (cwin->p, ATTR_HIGHEST);
  mvwprintw (cwin->p, Y, XFORE + XOFST, fore_txt);
  mvwprintw (cwin->p, Y, XBACK + XOFST, back_txt);
  custom_remove_attr (cwin->p, ATTR_HIGHEST);

  if (colorize)
    {
      mvwaddch (cwin->p, pos[*mark_fore][YPOS],
		pos[*mark_fore][XPOS] + 1, MARK);
      mvwaddch (cwin->p, pos[*mark_back][YPOS],
		pos[*mark_back][XPOS] + 1, MARK);
    }

  mvwaddch (cwin->p, pos[cursor][YPOS], pos[cursor][XPOS] + 1, CURSOR);
  status_mesg (choose_color_1, choose_color_2);
  wnoutrefresh (win[STA].p);
  wnoutrefresh (cwin->p);
  doupdate ();
  if (notify_bar ())
    notify_update_bar ();
}

/* Color theme configuration. */
void
custom_color_config (void)
{
  window_t conf_win;
  int ch, cursor, need_reset, theme_changed;
  int mark_fore, mark_back;

  mark_fore = NBUSERCOLORS;
  mark_back = SIZE - 1;
  clear ();
  cursor = 0;
  need_reset = 1;
  theme_changed = 0;
  conf_win.p = NULL;
  display_color_config (&conf_win, &mark_fore, &mark_back, cursor,
			need_reset, theme_changed);

  while ((ch = wgetch (win[STA].p)) != 'q')
    {
      need_reset = 0;
      theme_changed = 0;

      switch (ch)
	{
	case KEY_RESIZE:
	  endwin ();
	  refresh ();
	  curs_set (0);
	  need_reset = 1;
	  break;

	case SPACE:
	case 'X':
	case 'x':
	  colorize = true;
	  need_reset = 1;
	  theme_changed = 1;
	  if (cursor > NBUSERCOLORS)
	    mark_back = cursor;
	  else
	    mark_fore = cursor;
	  break;

	case 258:
	case 'J':
	case 'j':
	  if (cursor < SIZE - 1)
	    ++cursor;
	  break;

	case 259:
	case 'K':
	case 'k':
	  if (cursor > 0)
	    --cursor;
	  break;

	case 260:
	case 'H':
	case 'h':
	  if (cursor > NBUSERCOLORS)
	    cursor -= (NBUSERCOLORS + 1);
	  break;

	case 261:
	case 'L':
	case 'l':
	  if (cursor <= NBUSERCOLORS)
	    cursor += (NBUSERCOLORS + 1);
	  break;

	case '0':
	  colorize = false;
	  need_reset = 1;
	  break;
	}
      display_color_config (&conf_win, &mark_fore, &mark_back, cursor,
			    need_reset, theme_changed);
    }
  delwin (conf_win.p);
}

/* 
 * Return a string defining the color theme in the form:
 *       foreground color 'on' background color
 * in order to dump this data in the configuration file.
 * Color numbers follow the ncurses library definitions. 
 * If ncurses library was compiled with --enable-ext-funcs,
 * then default color is -1.
 */
void
custom_color_theme_name (char *theme_name)
{
#define MAXCOLORS		8
#define NBCOLORS		2
#define DEFAULTCOLOR		255
#define DEFAULTCOLOR_EXT	-1

  int i;
  short color[NBCOLORS];
  char *color_name[NBCOLORS];
  char *default_color = "default";
  char *name[MAXCOLORS] = {
    "black",
    "red",
    "green",
    "yellow",
    "blue",
    "magenta",
    "cyan",
    "white"
  };
  const char *error_txt =
    _("FATAL ERROR in custom_color_theme_name: unknown color\n");

  if (!colorize)
    snprintf (theme_name, BUFSIZ, "0");
  else
    {
      pair_content (COLR_CUSTOM, &color[0], &color[1]);
      for (i = 0; i < NBCOLORS; i++)
	{
	  if ((color[i] == DEFAULTCOLOR) || (color[i] == DEFAULTCOLOR_EXT))
	    color_name[i] = default_color;
	  else if (color[i] >= 0 && color[i] <= MAXCOLORS)
	    color_name[i] = name[color[i]];
	  else
	    {
	      fputs (error_txt, stderr);
	      exit (EXIT_FAILURE);
	      /* NOTREACHED */
	    }
	}
      snprintf (theme_name, BUFSIZ, "%s on %s", color_name[0], color_name[1]);
    }
}

/* Prints the general options. */
static void
custom_print_general_options (WINDOW *optwin, conf_t *conf)
{
  int x_pos, y_pos;
  char *option1 = _("auto_save = ");
  char *option2 = _("confirm_quit = ");
  char *option3 = _("confirm_delete = ");
  char *option4 = _("skip_system_dialogs = ");
  char *option5 = _("skip_progress_bar = ");
  char *option6 = _("week_begins_on_monday = ");
  char *option7 = _("output_datefmt = ");
  char *option8 = _("input_datefmt = ");

  x_pos = 3;
  y_pos = 3;

  mvwprintw (optwin, y_pos, x_pos, "[1] %s      ", option1);
  print_option_incolor (optwin, conf->auto_save, y_pos,
			x_pos + 4 + strlen (option1));
  mvwprintw (optwin, y_pos + 1, x_pos,
	     _("(if set to YES, automatic save is done when quitting)"));

  mvwprintw (optwin, y_pos + 3, x_pos, "[2] %s      ", option2);
  print_option_incolor (optwin, conf->confirm_quit, y_pos + 3,
			x_pos + 4 + strlen (option2));
  mvwprintw (optwin, y_pos + 4, x_pos,
	     _("(if set to YES, confirmation is required before quitting)"));

  mvwprintw (optwin, y_pos + 6, x_pos, "[3] %s      ", option3);
  print_option_incolor (optwin, conf->confirm_delete, y_pos + 6,
			x_pos + 4 + strlen (option3));
  mvwprintw (optwin, y_pos + 7, x_pos,
	     _("(if set to YES, confirmation is required "
               "before deleting an event)"));

  mvwprintw (optwin, y_pos + 9, x_pos, "[4] %s      ", option4);
  print_option_incolor (optwin, conf->skip_system_dialogs, y_pos + 9,
			x_pos + 4 + strlen (option4));
  mvwprintw (optwin, y_pos + 10, x_pos,
	     _("(if set to YES, messages about loaded "
               "and saved data will not be displayed)"));

  mvwprintw (optwin, y_pos + 12, x_pos, "[5] %s      ", option5);
  print_option_incolor (optwin, conf->skip_progress_bar, y_pos + 12,
			x_pos + 4 + strlen (option5));
  mvwprintw (optwin, y_pos + 13, x_pos,
	     _("(if set to YES, progress bar will not be displayed "
                "when saving data)"));

  mvwprintw (optwin, y_pos + 15, x_pos, "[6] %s      ", option6);
  print_option_incolor (optwin, calendar_week_begins_on_monday (), y_pos + 15,
			x_pos + 4 + strlen (option6));
  mvwprintw (optwin, y_pos + 16, x_pos,
	     _("(if set to YES, monday is the first day of the week, "
               "else it is sunday)"));

  mvwprintw (optwin, y_pos + 18, x_pos, "[7] %s      ", option7);
  custom_apply_attr (optwin, ATTR_HIGHEST);
  mvwprintw (optwin, y_pos + 18, x_pos + 4 + strlen (option7), "%s",
	     conf->output_datefmt);
  custom_remove_attr (optwin, ATTR_HIGHEST);
  mvwprintw (optwin, y_pos + 19, x_pos,
	     _("(Format of the date to be displayed in non-interactive mode)"));

  mvwprintw (optwin, y_pos + 21, x_pos, "[8] %s      ", option8);
  custom_apply_attr (optwin, ATTR_HIGHEST);
  mvwprintw (optwin, y_pos + 21, x_pos + 4 + strlen (option7), "%d",
	     conf->input_datefmt);
  custom_remove_attr (optwin, ATTR_HIGHEST);
  mvwprintw (optwin, y_pos + 22, x_pos,
	     _("(Format to be used when entering a date: "
               "1-mm/dd/yyyy, 2-dd/mm/yyyy, 3-yyyy/mm/dd)"));

  wmove (win[STA].p, 1, 0);
  wnoutrefresh (optwin);
  doupdate ();
}

/* General configuration. */
void
custom_general_config (conf_t *conf)
{
  window_t conf_win;
  char *number_str =
    _("Enter an option number to change its value [Q to quit] ");
  char *output_datefmt_str =
    _("Enter the date format (see 'man 3 strftime' for possible formats) ");
  char *input_datefmt_str =
    _("Enter the date format (1-mm/dd/yyyy, 2-dd/mm/yyyy, 3-yyyy/mm/dd) ");
  int ch;
  char label[BUFSIZ];
  char *buf = (char *) malloc (BUFSIZ);

  clear ();
  snprintf (label, BUFSIZ, _("CalCurse %s | general options"), VERSION);
  custom_confwin_init (&conf_win, label);
  status_mesg (number_str, "");
  custom_print_general_options (conf_win.p, conf);
  while ((ch = wgetch (win[STA].p)) != 'q')
    {
      switch (ch)
	{
	case KEY_RESIZE:
	  endwin ();
	  refresh ();
	  curs_set (0);
	  delwin (conf_win.p);
	  custom_confwin_init (&conf_win, label);
	  delwin (win[STA].p);
	  win[STA].p = newwin (win[STA].h, win[STA].w, win[STA].y,
			       win[STA].x);
	  keypad (win[STA].p, TRUE);
	  if (notify_bar ())
	    {
	      notify_reinit_bar (win[NOT].h, win[NOT].w, win[NOT].y,
                                 win[NOT].x);
	      notify_update_bar ();
	    }
	  break;
	case '1':
	  conf->auto_save = !conf->auto_save;
	  break;
	case '2':
	  conf->confirm_quit = !conf->confirm_quit;
	  break;
	case '3':
	  conf->confirm_delete = !conf->confirm_delete;
	  break;
	case '4':
	  conf->skip_system_dialogs = !conf->skip_system_dialogs;
	  break;
	case '5':
	  conf->skip_progress_bar = !conf->skip_progress_bar;
	  break;
	case '6':
	  calendar_change_first_day_of_week ();
	  break;
	case '7':
	  status_mesg (output_datefmt_str, "");
	  strncpy (buf, conf->output_datefmt,
		   strlen (conf->output_datefmt) + 1);
	  if (updatestring (win[STA].p, &buf, 0, 1) == 0)
	    {
	      strncpy (conf->output_datefmt, buf, strlen (buf) + 1);
	    }
	  status_mesg (number_str, "");
	  break;
	case '8':
	  status_mesg (input_datefmt_str, "");
	  if (updatestring (win[STA].p, &buf, 0, 1) == 0)
	    {
	      int val = atoi (buf);
	      if (val >= 1 && val <= 3)
		conf->input_datefmt = val;
	    }
	  status_mesg (number_str, "");
	  break;
	}
      status_mesg (number_str, "");
      custom_print_general_options (conf_win.p, conf);
    }
  free (buf);
  delwin (conf_win.p);
}

/*	$calcurse: event.c,v 1.7 2008/04/12 21:14:03 culot Exp $	*/

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

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "vars.h"
#include "i18n.h"
#include "event.h"

struct event_s *eventlist;

/* Create a new event */
struct event_s *
event_new (char *mesg, char *note, long day, int id)
{
  struct event_s *o, **i;
  o = (struct event_s *) malloc (sizeof (struct event_s));
  o->mesg = (char *) malloc (strlen (mesg) + 1);
  strncpy (o->mesg, mesg, strlen (mesg) + 1);
  o->day = day;
  o->id = id;
  o->note = (note != NULL) ? strdup (note) : NULL;
  i = &eventlist;
  for (;;)
    {
      if (*i == 0 || (*i)->day > day)
	{
	  o->next = *i;
	  *i = o;
	  break;
	}
      i = &(*i)->next;
    }
  return (o);
}

/* Check if the event belongs to the selected day */
unsigned
event_inday (struct event_s *i, long start)
{
  if (i->day <= start + DAYINSEC && i->day > start)
    {
      return (1);
    }
  return (0);
}

/* Write to file the event in user-friendly format */
void
event_write (struct event_s *o, FILE *f)
{
  struct tm *lt;
  time_t t;

  t = o->day;
  lt = localtime (&t);
  fprintf (f, "%02u/%02u/%04u [%d] ", lt->tm_mon + 1, lt->tm_mday,
           1900 + lt->tm_year, o->id);
  if (o->note != NULL)
    fprintf (f, ">%s ", o->note);
  fprintf (f, "%s\n", o->mesg);
}

/* Load the events from file */
struct event_s *
event_scan (FILE *f, struct tm start, int id, char *note)
{
  struct tm *lt;
  char buf[MESG_MAXSIZE], *nl;
  time_t tstart, t;

  t = time (NULL);
  lt = localtime (&t);

  /* Read the event description */
  fgets (buf, MESG_MAXSIZE, f);
  nl = strchr (buf, '\n');
  if (nl)
    {
      *nl = '\0';
    }
  start.tm_hour = 12;
  start.tm_min = 0;
  start.tm_sec = 0;
  start.tm_isdst = -1;
  start.tm_year -= 1900;
  start.tm_mon--;

  tstart = mktime (&start);
  if (tstart == -1)
    {
      fputs (_("FATAL ERROR in event_scan: date error in the event\n"),
	     stderr);
      exit (EXIT_FAILURE);
    }
  return (event_new (buf, note, tstart, id));
}

/* Retrieve an event from the list, given the day and item position. */
struct event_s *
event_get (long day, int pos)
{
  struct event_s *o;
  int n;

  n = 0;
  for (o = eventlist; o; o = o->next)
    {
      if (event_inday (o, day))
	{
	  if (n == pos)
	    return o;
	  n++;
	}
    }
  /* NOTREACHED */
  fputs (_("FATAL ERROR in event_get: no such item\n"), stderr);
  exit (EXIT_FAILURE);
}

/* Delete an event from the list. */
void
event_delete_bynum (long start, unsigned num, erase_flag_e flag)
{
  unsigned n;
  struct event_s *i, **iptr;

  n = 0;
  iptr = &eventlist;
  for (i = eventlist; i != 0; i = i->next)
    {
      if (event_inday (i, start))
	{
	  if (n == num)
	    {
	      if (flag == ERASE_FORCE_ONLY_NOTE)
		erase_note (&i->note, flag);
	      else
		{
		  *iptr = i->next;
		  free (i->mesg);
		  erase_note (&i->note, flag);
		  free (i);
		}
	      return;
	    }
	  n++;
	}
      iptr = &i->next;
    }
  /* NOTREACHED */
  fputs (_("FATAL ERROR in event_delete_bynum: no such event\n"), stderr);
  exit (EXIT_FAILURE);
}

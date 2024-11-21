#include "table.h"
#include "stack.h"
#include "common.h"

static int
get_table_dim (char *table_buf)
{
  int dim = 0;
  char cur_pointer;
  cur_pointer = *table_buf;
  while (cur_pointer != '\n')
  {
    if (cur_pointer != ',')
      dim++;
    table_buf++;
    cur_pointer = *table_buf;
  }
  return dim;
}

char *
produce_table ()
{
  char *latex_buf;
  char *table_buf;
  char *path_buf;
  int dim;
  const char *path_start;
  int path_len = 0;

  Token token = pop_token ();
  /*  FF whitespace.  */
  while (token.type != TOKEN_TABLE_PATH)
    token = pop_token ();
  /*  Get the path.  */
  path_start = token.start;
  path_len += token.length;
  while (peek_next(0).type == TOKEN_TABLE_PATH)
    {
      token = pop_token ();
      path_len += token.length;
    }

  /*  Collate path.  */  
  path_buf = malloc (sizeof (char) * (path_len + 1));
  sprintf (path_buf, "%.*s", path_len, path_start);

  table_buf = read_file (path_buf);
  dim = get_table_dim (table_buf);

  char *table_preamble = "\\begin{table}[]\\begin{tabular}{|";
  char *table_postamble = " \\hline\\end{tabular} ";
  // TODO: fix this
  latex_buf = calloc (strlen (table_buf) + strlen (table_preamble) + strlen (table_postamble) + 1000, sizeof (char));

  /*  Start writing table into latex buffer.  */
  int buf_ptr = strlen(table_preamble);
  strcat(latex_buf, table_preamble);
  for (int i = 0; i < dim; i++)
    {
      strcat (latex_buf, "l|");
      buf_ptr += 2;
    }
  strcat (latex_buf, "} \\hline ");
  buf_ptr += strlen ("} \\hline ");
  /*  Read table info into buffer.  */
  char *tmp = table_buf;
  while (*tmp != '\0')
    {
      if (*tmp == ',')
        {
          latex_buf[buf_ptr] = '&';
          buf_ptr++;
        }
      else if (*tmp == '\n')
        {
          strcat (latex_buf, " \\\\ \\hline ");
          buf_ptr += strlen (" \\\\ \\hline ");
        }
      else
        {
          latex_buf[buf_ptr] = *tmp;
          buf_ptr++;
        }
      tmp++;
    }
  /*  Complete line.  */
  strcat (latex_buf, table_postamble);
  strcat (latex_buf, "%table ");
  strcat (latex_buf, path_buf);
  return latex_buf;
}
/****************************************************************************
* [S]imulated [M]edieval [A]dventure multi[U]ser [G]ame      |   \\._.//   *
* -----------------------------------------------------------|   (0...0)   *
* SMAUG 1.0 (C) 1994, 1995, 1996, 1998  by Derek Snider      |    ).:.(    *
* -----------------------------------------------------------|    {o o}    *
* SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,      |   / ' ' \   *
* Scryn, Rennard, Swordbearer, Gorog, Grishnakh, Nivek,      |~'~.VxvxV.~'~*
* Tricops and Fireblade                                      |             *
* ------------------------------------------------------------------------ *
* Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        *
* Chastain, Michael Quan, and Mitchell Tse.                                *
* Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          *
* Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.     *
* ------------------------------------------------------------------------ *
*			   Wizard/god command module			    *
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>
#include "mud.h"
#include "rank.h"
#ifdef WIN32
#include <io.h>
#define F_OK 0
#endif


#define RESTORE_INTERVAL 21600

char *const save_flag[] = { "death", "kill", "passwd", "drop", "put", "give", "auto", "zap",
  "auction", "get", "receive", "idle", "backup", "quitbackup", "fill",
  "empty", "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", "r24",
  "r25", "r26", "r27", "r28", "r29", "r30", "r31"
};

bool add_hiscore( char *keyword, char *name, int score );
bool add_hiscore_ld( char *keyword, char *name, long double score );
/* from reset.c */
int generate_itemlevel args( ( AREA_DATA * pArea, OBJ_INDEX_DATA * pObjIndex ) );

/* from comm.c */
bool write_to_descriptor args( ( int desc, char *txt, int length ) );
bool check_parse_name args( ( char *name, bool newchar ) );

/* from boards.c */
void note_attach( CHAR_DATA * ch );

/* from build.c */
int get_risflag( char *flag );
int get_defenseflag( char *flag );
int get_attackflag( char *flag );

/* from tables.c */
void write_race_file( int ra );

/*
 * Local functions.
 */
// long double atod args( (const char* szNumber) );
long double atod args( ( const char *string ) );

char *tiny_affect_loc_name( int location );

ROOM_INDEX_DATA *find_location args( ( CHAR_DATA * ch, char *arg ) );
void save_watchlist args( ( void ) );
void save_banlist args( ( void ) );
void close_area args( ( AREA_DATA * pArea ) );

int get_color( char *argument );  /* function proto */

void sort_reserved args( ( RESERVE_DATA * pRes ) );

PROJECT_DATA *get_project_by_number args( ( int pnum ) );
NOTE_DATA *get_log_by_number args( ( PROJECT_DATA * pproject, int pnum ) );


/*
 * Global variables.
 */

char reboot_time[50];
time_t new_boot_time_t;
extern struct tm new_boot_struct;
extern OBJ_INDEX_DATA *obj_index_hash[MAX_KEY_HASH];
extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];

int get_saveflag( char *name )
{
  int x;

  for( x = 0; x < sizeof( save_flag ) / sizeof( save_flag[0] ); x++ )
    if( !str_cmp( name, save_flag[x] ) )
      return x;
  return -1;
}

/*
 * Toggle "Do Not Disturb" flag. Used to prevent lower level imms from
 * using commands like "trans" and "goto" on higher level imms.
 */
void do_dnd( CHAR_DATA * ch, char *argument )
{
  if( !IS_NPC( ch ) && ch->pcdata )
    if( IS_SET( ch->pcdata->flags, PCFLAG_DND ) )
    {
      REMOVE_BIT( ch->pcdata->flags, PCFLAG_DND );
      send_to_char( "Your 'do not disturb' flag is now off.\n\r", ch );
    }
    else
    {
      SET_BIT( ch->pcdata->flags, PCFLAG_DND );
      send_to_char( "Your 'do not disturb' flag is now on.\n\r", ch );
    }
  else
    send_to_char( "huh?\n\r", ch );
}

void do_defuse( CHAR_DATA * ch, char *argument )
{
/*Will do later*/
}

void do_showimmlog( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char lstart[MAX_INPUT_LENGTH];
  char lcount[MAX_INPUT_LENGTH];
  char filename[MAX_INPUT_LENGTH];
  char s[MAX_STRING_LENGTH];
  FILE *fp;
  int start = 0;
  int numlines = 0;
  int x = 0;
  int y = 0;

  if( IS_NPC( ch ) )
    return;

  argument = one_argument( argument, arg );
  argument = one_argument( argument, lstart );
  argument = one_argument( argument, lcount );

  set_pager_color( AT_IMMORT, ch );

  if( arg[0] == '\0' || lstart[0] == '\0' || lcount[0] == '\0' || !is_number( lstart ) || !is_number( lcount ) )
  {
    send_to_pager( "   showimmlog <target> <starting line> <number of lines>\n\r", ch );
    return;
  }
  else
  {
    start = atoi( lstart );
    numlines = atoi( lcount );

    set_pager_color( AT_PLAIN, ch );
    strcat( arg, "log" );
    sprintf( filename, "%s%s", WATCH_DIR, strlower( arg ) );
    if( !( fp = fopen( filename, "r" ) ) )
    {
      send_to_char( "Invalid filename.\n\r", ch );
      return;
    }
    else
    {
      fgets( s, MAX_STRING_LENGTH, fp );
      x++;
      while( !feof( fp ) )
      {
//  set_pager_color( AT_PLAIN, ch );
//  send_to_pager( s, ch );
        fgets( s, MAX_STRING_LENGTH, fp );
        x++;
      }
      fclose( fp );
      if( x < start + numlines )
      {
        ch_printf( ch, "The line range must be within %d lines.\n\r", x );
        return;
      }
      else
      {
        fp = fopen( filename, "r" );
        for( x = 0; x < start; x++ )
          fgets( s, MAX_STRING_LENGTH, fp );
        for( y = 0; y < numlines; y++ )
        {
          ch_printf( ch, "&R&w%d %s", x + y, s );
          fgets( s, MAX_STRING_LENGTH, fp );
        }
        send_to_pager( "\n\r", ch );
      }
      fclose( fp );
    }
  }
}

/*
 * The "watch" facility allows imms to specify the name of a player or
 * the name of a site to be watched. It is like "logging" a player except
 * the results are written to a file in the "watch" directory named with
 * the same name as the imm. The idea is to allow lower level imms to
 * watch players or sites without having to have access to the log files.
 */
void do_watch( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  WATCH_DATA *pw;

  if( IS_NPC( ch ) )
    return;

  argument = one_argument( argument, arg );
  set_pager_color( AT_IMMORT, ch );

  if( arg[0] == '\0' || !str_cmp( arg, "help" ) )
  {
    send_to_pager( "Syntax Examples:\n\r", ch );
    /*
     * Only IMP+ can see all the watches. The rest can just see their own.
     */
    if( get_trust( ch ) >= LEVEL_IMPLEMENTOR )
      send_to_pager( "   watch show all          show all watches\n\r", ch );
    send_to_pager( "   watch show              show all my watches\n\r"
                   "   watch size              show the size of my watch file\n\r"
                   "   watch player joe        add a new player watch\n\r"
                   "   watch site 2.3.123      add a new site watch\n\r"
                   "   watch command make      add a new command watch\n\r"
                   "   watch site 2.3.12       matches 2.3.12x\n\r"
                   "   watch site 2.3.12.      matches 2.3.12.x\n\r"
                   "   watch delete n          delete my nth watch\n\r"
                   "   watch print 500         print watch file starting at line 500\n\r"
                   "   watch print 500 1000    print 1000 lines starting at line 500\n\r"
                   "   watch clear             clear my watch file\n\r", ch );
    return;
  }

  set_pager_color( AT_PLAIN, ch );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  /*
   * Clear watch file
   */
  if( !str_cmp( arg, "clear" ) )
  {
    char fname[MAX_INPUT_LENGTH];

    sprintf( fname, "%s%s", WATCH_DIR, strlower( ch->name ) );
    if( 0 == remove( fname ) )
    {
      send_to_pager( "Ok. Your watch file has been cleared.\n\r", ch );
      return;
    }
    send_to_pager( "You have no valid watch file to clear.\n\r", ch );
    return;
  }

  /*
   * Display size of watch file
   */
  if( !str_cmp( arg, "size" ) )
  {
    FILE *fp;
    char fname[MAX_INPUT_LENGTH], s[MAX_STRING_LENGTH];
    int rec_count = 0;

    sprintf( fname, "%s%s", WATCH_DIR, strlower( ch->name ) );

    if( !( fp = fopen( fname, "r" ) ) )
    {
      send_to_pager( "You have no watch file. Perhaps you cleared it?\n\r", ch );
      return;
    }

    fgets( s, MAX_STRING_LENGTH, fp );
    while( !feof( fp ) )
    {
      rec_count++;
      fgets( s, MAX_STRING_LENGTH, fp );
    }
    pager_printf( ch, "You have %d lines in your watch file.\n\r", rec_count );
    fclose( fp );
    return;
  }

  /*
   * Print watch file
   */
  if( !str_cmp( arg, "print" ) )
  {
    FILE *fp;
    char fname[MAX_INPUT_LENGTH], s[MAX_STRING_LENGTH];
    const int MAX_DISPLAY_LINES = 1000;
    int start, limit, disp_count = 0, rec_count = 0;

    if( arg2[0] == '\0' )
    {
      send_to_pager( "Sorry. You must specify a starting line number.\n\r", ch );
      return;
    }

    start = atoi( arg2 );
    limit = ( arg3[0] == '\0' ) ? MAX_DISPLAY_LINES : atoi( arg3 );
    limit = UMIN( limit, MAX_DISPLAY_LINES );

    sprintf( fname, "%s%s", WATCH_DIR, strlower( ch->name ) );
    if( !( fp = fopen( fname, "r" ) ) )
      return;
    fgets( s, MAX_STRING_LENGTH, fp );

    while( ( disp_count < limit ) && ( !feof( fp ) ) )
    {
      if( ++rec_count >= start )
      {
        send_to_pager( s, ch );
        disp_count++;
      }
      fgets( s, MAX_STRING_LENGTH, fp );
    }
    send_to_pager( "\n\r", ch );
    if( disp_count >= MAX_DISPLAY_LINES )
      send_to_pager( "Maximum display lines exceeded. List is terminated.\n\r"
                     "Type 'help watch' to see how to print the rest of the list.\n\r"
                     "\n\r" "Your watch file is large. Perhaps you should clear it?\n\r", ch );

    fclose( fp );
    return;
  }

  /*
   * Display all watches
   * Only IMP+ can see all the watches. The rest can just see their own.
   */
  if( get_trust( ch ) >= LEVEL_IMPLEMENTOR && !str_cmp( arg, "show" ) && !str_cmp( arg2, "all" ) )
  {
    pager_printf( ch, "%-12s %-14s %-15s\n\r", "Imm Name", "Player/Command", "Player Site" );
    if( first_watch )
      for( pw = first_watch; pw; pw = pw->next )
        if( get_trust( ch ) >= pw->imm_level )
          pager_printf( ch, "%-14s %-12s %-15s\n\r",
                        pw->imm_name, pw->target_name ? pw->target_name : " ", pw->player_site ? pw->player_site : " " );
    return;
  }

  /*
   * Display only those watches belonging to the requesting imm
   */
  if( !str_cmp( arg, "show" ) && arg2[0] == '\0' )
  {
    int cou = 0;
    pager_printf( ch, "%-3s %-12s %-14s %-15s\n\r", " ", "Imm Name", "Player/Command", "Player Site" );
    if( first_watch )
      for( pw = first_watch; pw; pw = pw->next )
        if( !str_cmp( ch->name, pw->imm_name ) )
          pager_printf( ch, "%3d %-12s %-14s %-15s\n\r",
                        ++cou, pw->imm_name, pw->target_name ? pw->target_name : " ",
                        pw->player_site ? pw->player_site : " " );
    return;
  }

  /*
   * Delete a watch belonging to the requesting imm
   */
  if( !str_cmp( arg, "delete" ) && isdigit( *arg2 ) )
  {
    int cou = 0;
    int num;

    num = atoi( arg2 );
    if( first_watch )
      for( pw = first_watch; pw; pw = pw->next )
        if( !str_cmp( ch->name, pw->imm_name ) )
          if( num == ++cou )
          {
            /*
             * Oops someone forgot to clear up the memory --Shaddai 
             */
            if( pw->imm_name )
              DISPOSE( pw->imm_name );
            if( pw->player_site )
              DISPOSE( pw->player_site );
            if( pw->target_name )
              DISPOSE( pw->target_name );
            /*
             * Now we can unlink and then clear up that final
             * * pointer -- Shaddai
             */
            UNLINK( pw, first_watch, last_watch, next, prev );
            DISPOSE( pw );
            save_watchlist(  );
            send_to_pager( "Deleted.\n\r", ch );
            return;
          }
    send_to_pager( "Sorry. I found nothing to delete.\n\r", ch );
    return;
  }

  /*
   * Watch a specific player
   */
  if( !str_cmp( arg, "player" ) && *arg2 )
  {
    WATCH_DATA *pinsert;
    CHAR_DATA *vic;
    char buf[MAX_INPUT_LENGTH];

    if( first_watch ) /* check for dups */
      for( pw = first_watch; pw; pw = pw->next )
        if( !str_cmp( ch->name, pw->imm_name ) && pw->target_name && !str_cmp( arg2, pw->target_name ) )
        {
          send_to_pager( "You are already watching that player.\n\r", ch );
          return;
        }

    CREATE( pinsert, WATCH_DATA, 1 ); /* create new watch */
    pinsert->imm_level = get_trust( ch );
    pinsert->imm_name = str_dup( strlower( ch->name ) );
    pinsert->target_name = str_dup( strlower( arg2 ) );
    pinsert->player_site = NULL;

    /*
     * stupid get_char_world returns ptr to "samantha" when given "sam" 
     */
    /*
     * so I do a str_cmp to make sure it finds the right player --Gorog 
     */

    sprintf( buf, "0.%s", arg2 );
    if( ( vic = get_char_world( ch, buf ) ) ) /* if vic is in game now */
      if( ( !IS_NPC( vic ) ) && !str_cmp( arg2, vic->name ) )
        SET_BIT( vic->pcdata->flags, PCFLAG_WATCH );

    if( first_watch ) /* ins new watch if app */
      for( pw = first_watch; pw; pw = pw->next )
        if( strcmp( pinsert->imm_name, pw->imm_name ) < 0 )
        {
          INSERT( pinsert, pw, first_watch, next, prev );
          save_watchlist(  );
          send_to_pager( "Ok. That player will be watched.\n\r", ch );
          return;
        }

    LINK( pinsert, first_watch, last_watch, next, prev ); /* link new watch */
    save_watchlist(  );
    send_to_pager( "Ok. That player will be watched.\n\r", ch );
    return;
  }

  /*
   * Watch a specific site
   */
  if( !str_cmp( arg, "site" ) && *arg2 )
  {
    WATCH_DATA *pinsert;
    CHAR_DATA *vic;

    if( first_watch ) /* check for dups */
      for( pw = first_watch; pw; pw = pw->next )
        if( !str_cmp( ch->name, pw->imm_name ) && pw->player_site && !str_cmp( arg2, pw->player_site ) )
        {
          send_to_pager( "You are already watching that site.\n\r", ch );
          return;
        }

    CREATE( pinsert, WATCH_DATA, 1 ); /* create new watch */
    pinsert->imm_level = get_trust( ch );
    pinsert->imm_name = str_dup( strlower( ch->name ) );
    pinsert->player_site = str_dup( strlower( arg2 ) );
    pinsert->target_name = NULL;

    for( vic = first_char; vic; vic = vic->next )
      if( !IS_NPC( vic ) && vic->desc && *pinsert->player_site
          && !str_prefix( pinsert->player_site, vic->desc->host ) && get_trust( vic ) < pinsert->imm_level )
        SET_BIT( vic->pcdata->flags, PCFLAG_WATCH );

    if( first_watch ) /* ins new watch if app */
      for( pw = first_watch; pw; pw = pw->next )
        if( strcmp( pinsert->imm_name, pw->imm_name ) < 0 )
        {
          INSERT( pinsert, pw, first_watch, next, prev );
          save_watchlist(  );
          send_to_pager( "Ok. That site will be watched.\n\r", ch );
          return;
        }

    LINK( pinsert, first_watch, last_watch, next, prev );
    save_watchlist(  );
    send_to_pager( "Ok. That site will be watched.\n\r", ch );
    return;
  }

  /*
   * Watch a specific command - FB
   */
  if( !str_cmp( arg, "command" ) && *arg2 )
  {
    WATCH_DATA *pinsert;
    CMDTYPE *cmd;
    bool found = FALSE;

    for( pw = first_watch; pw; pw = pw->next )
    {
      if( !str_cmp( ch->name, pw->imm_name ) && pw->target_name && !str_cmp( arg2, pw->target_name ) )
      {
        send_to_pager( "You are already watching that command.\n\r", ch );
        return;
      }
    }

    for( cmd = command_hash[LOWER( arg2[0] ) % 126]; cmd; cmd = cmd->next )
    {
      if( !strcmp( arg2, cmd->name ) )
      {
        found = TRUE;
        break;
      }
    }

    if( !found )
    {
      send_to_pager( "No such command exists.\n\r", ch );
      return;
    }
    else
    {
      SET_BIT( cmd->flags, CMD_WATCH );
    }

    CREATE( pinsert, WATCH_DATA, 1 );
    pinsert->imm_level = get_trust( ch );
    pinsert->imm_name = str_dup( strlower( ch->name ) );
    pinsert->player_site = NULL;
    pinsert->target_name = str_dup( arg2 );

    for( pw = first_watch; pw; pw = pw->next )
    {
      if( strcmp( pinsert->imm_name, pw->imm_name ) < 0 )
      {
        INSERT( pinsert, pw, first_watch, next, prev );
        save_watchlist(  );
        send_to_pager( "Ok, That command will be watched.\n\r", ch );
        return;
      }
    }

    LINK( pinsert, first_watch, last_watch, next, prev );
    save_watchlist(  );
    send_to_pager( "Ok. That site will be watched.\n\r", ch );
    return;
  }

  send_to_pager( "Sorry. I can't do anything with that. " "Please read the help file.\n\r", ch );
  return;
}

void do_wizhelp( CHAR_DATA * ch, char *argument )
{
  CMDTYPE *cmd;
  int col, hash;
  int curr_lvl;
  col = 0;

  for( curr_lvl = LEVEL_AVATAR; curr_lvl <= get_trust( ch ); curr_lvl++ )
  {
    col = 0;
    pager_printf( ch, "\n\r\n\r&W[LEVEL %-2d]:&w\n\r", curr_lvl );
    for( hash = 0; hash < 126; hash++ )
      for( cmd = command_hash[hash]; cmd; cmd = cmd->next )
        if( ( cmd->level == curr_lvl ) && cmd->level <= get_trust( ch ) )
        {
          pager_printf( ch, "%-15s", cmd->name );
          if( ++col == 5 )
          {
            col = 0;
            send_to_pager( "\n\r", ch );
          }
        }
  }
  pager_printf( ch, "\n\r" );
  return;
}

void do_restrict( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  sh_int level, hash;
  CMDTYPE *cmd;
  bool found;

  found = FALSE;
  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Restrict which command?\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg2 );
  if( arg2[0] == '\0' )
    level = get_trust( ch );
  else
    level = atoi( arg2 );

  level = UMAX( UMIN( get_trust( ch ), level ), 0 );

  hash = arg[0] % 126;
  for( cmd = command_hash[hash]; cmd; cmd = cmd->next )
  {
    if( !str_prefix( arg, cmd->name ) && cmd->level <= get_trust( ch ) )
    {
      found = TRUE;
      break;
    }
  }

  if( found )
  {
    if( !str_prefix( arg2, "show" ) )
    {
      sprintf( buf, "%s show", cmd->name );
      do_cedit( ch, buf );
      /*
       * ch_printf( ch, "%s is at level %d.\n\r", cmd->name, cmd->level );
       */
      return;
    }
    cmd->level = level;
    ch_printf( ch, "You restrict %s to level %d\n\r", cmd->name, level );
    sprintf( buf, "%s restricting %s to level %d", ch->name, cmd->name, level );
    log_string( buf );
  }
  else
    send_to_char( "You may not restrict that command.\n\r", ch );

  return;
}

/*
 * Check if the name prefix uniquely identifies a char descriptor
 */
CHAR_DATA *get_waiting_desc( CHAR_DATA * ch, char *name )
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *ret_char = NULL;
  static unsigned int number_of_hits;

  number_of_hits = 0;
  for( d = first_descriptor; d; d = d->next )
  {
    if( d->character && ( !str_prefix( name, d->character->name ) ) && IS_WAITING_FOR_AUTH( d->character ) )
    {
      if( ++number_of_hits > 1 )
      {
        ch_printf( ch, "%s does not uniquely identify a char.\n\r", name );
        return NULL;
      }
      ret_char = d->character;  /* return current char on exit */
    }
  }
  if( number_of_hits == 1 )
    return ret_char;
  else
  {
    send_to_char( "No one like that waiting for authorization.\n\r", ch );
    return NULL;
  }
}

void do_bamfin( CHAR_DATA * ch, char *argument )
{
  if( !IS_NPC( ch ) )
  {
    smash_tilde( argument );
    DISPOSE( ch->pcdata->bamfin );
    ch->pcdata->bamfin = str_dup( argument );
    send_to_char_color( "&YBamfin set.\n\r", ch );
  }
  return;
}

void do_bamfout( CHAR_DATA * ch, char *argument )
{
  if( !IS_NPC( ch ) )
  {
    smash_tilde( argument );
    DISPOSE( ch->pcdata->bamfout );
    ch->pcdata->bamfout = str_dup( argument );
    send_to_char_color( "&YBamfout set.\n\r", ch );
  }
  return;
}

void do_rank( CHAR_DATA * ch, char *argument )
{

  set_char_color( AT_IMMORT, ch );

  if( IS_NPC( ch ) )
    return;
  if( !argument || argument[0] == '\0' )
  {
    send_to_char( "Usage:  rank <string>.\n\r", ch );
    send_to_char( "   or:  rank none.\n\r", ch );
    return;
  }
  smash_tilde( argument );
  STRFREE( ch->pcdata->pretitle );
  if( !str_cmp( argument, "none" ) )
    ch->pcdata->pretitle = STRALLOC( "" );
  else
    ch->pcdata->pretitle = STRALLOC( argument );
  send_to_char( "Ok.\n\r", ch );
  return;
}

void do_retire( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Retire whom?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }
  if( get_trust( victim ) >= get_trust( ch ) )
  {
    send_to_char( "You failed.\n\r", ch );
    return;
  }
  if( victim->level < LEVEL_SAVIOR )
  {
    send_to_char( "The minimum level for retirement is savior.\n\r", ch );
    return;
  }
  if( IS_RETIRED( victim ) )
  {
    REMOVE_BIT( victim->pcdata->flags, PCFLAG_RETIRED );
    ch_printf( ch, "%s returns from retirement.\n\r", victim->name );
    ch_printf( victim, "%s brings you back from retirement.\n\r", ch->name );
  }
  else
  {
    SET_BIT( victim->pcdata->flags, PCFLAG_RETIRED );
    ch_printf( ch, "%s is now a retired immortal.\n\r", victim->name );
    ch_printf( victim, "Courtesy of %s, you are now a retired immortal.\n\r", ch->name );
  }
  return;
}

void do_delay( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  int delay;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( !*arg )
  {
    send_to_char( "Syntax:  delay <victim> <# of rounds>\n\r", ch );
    return;
  }
  if( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "No such character online.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Mobiles are unaffected by lag.\n\r", ch );
    return;
  }
  if( !IS_NPC( victim ) && get_trust( victim ) >= get_trust( ch ) )
  {
    send_to_char( "You haven't the power to succeed against them.\n\r", ch );
    return;
  }
  argument = one_argument( argument, arg );
  if( !*arg )
  {
    send_to_char( "For how long do you wish to delay them?\n\r", ch );
    return;
  }
  if( !str_cmp( arg, "none" ) )
  {
    send_to_char( "All character delay removed.\n\r", ch );
    victim->wait = 0;
    return;
  }
  delay = atoi( arg );
  if( delay < 1 )
  {
    send_to_char( "Pointless.  Try a positive number.\n\r", ch );
    return;
  }
  if( delay > 999 )
  {
    send_to_char( "You cruel bastard.  Just kill them.\n\r", ch );
    return;
  }
  WAIT_STATE( victim, delay * PULSE_VIOLENCE );
  ch_printf( ch, "You've delayed %s for %d rounds.\n\r", victim->name, delay );
  return;
}

void do_deny( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Deny whom?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }
  if( get_trust( victim ) >= get_trust( ch ) )
  {
    send_to_char( "You failed.\n\r", ch );
    return;
  }
  xSET_BIT( victim->act, PLR_DENY );
  set_char_color( AT_IMMORT, victim );
  send_to_char( "You are denied access!\n\r", victim );
  ch_printf( ch, "You have denied access to %s.\n\r", victim->name );
  if( victim->fighting )
    stop_fighting( victim, TRUE );  /* Blodkai, 97 */
  add_hiscore( "pkill", victim->name, 0 );
  add_hiscore( "sparwins", victim->name, 0 );
  add_hiscore( "sparloss", victim->name, 0 );
  add_hiscore( "mkills", victim->name, 0 );
  add_hiscore( "deaths", victim->name, 0 );
  add_hiscore_ld( "powerlevel", victim->name, 0 );
  if( is_saiyan( ch ) )
    add_hiscore_ld( "plsaiyan", victim->name, 0 );
  if( is_human( ch ) )
    add_hiscore_ld( "plhuman", victim->name, 0 );
  if( is_hb( ch ) )
    add_hiscore_ld( "plhalfbreed", victim->name, 0 );
  if( is_namek( ch ) )
    add_hiscore_ld( "plnamek", victim->name, 0 );
  if( is_android( ch ) )
    add_hiscore_ld( "plandroid", victim->name, 0 );
  if( IS_ICER( ch ) )
    add_hiscore_ld( "plicer", victim->name, 0 );
  if( is_bio( ch ) )
    add_hiscore_ld( "plbio-android", victim->name, 0 );
  if( IS_KAIO( ch ) )
    add_hiscore_ld( "plkaio", victim->name, 0 );
  if( IS_DEMON( ch ) )
    add_hiscore_ld( "pldemon", victim->name, 0 );
  if( is_genie( ch ) )
    add_hiscore_ld( "plgenie", victim->name, 0 );
  if( is_saibaman( ch ) )
    add_hiscore_ld( "plsaibaman", victim->name, 0 );
  if( is_fierian( ch ) )
    add_hiscore_ld( "plfierian", victim->name, 0 );
  if( is_hylian( ch ) )
    add_hiscore_ld( "plhylian", victim->name, 0 );
  if( is_reploid( ch ) )
    add_hiscore_ld( "plreploid", victim->name, 0 );
  add_hiscore( "played", victim->name, 0 );
  add_hiscore( "zeni", victim->name, 0 );
  add_hiscore( "bounty", victim->name, 0 );
  do_quit( victim, "" );
  return;
}

void do_disconnect( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Disconnect whom?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( victim->desc == NULL )
  {
    act( AT_PLAIN, "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
    return;
  }
  if( get_trust( ch ) <= get_trust( victim ) )
  {
    send_to_char( "They might not like that...\n\r", ch );
    return;
  }

  for( d = first_descriptor; d; d = d->next )
  {
    if( d == victim->desc )
    {
      close_socket( d, FALSE );
      send_to_char( "Ok.\n\r", ch );
      return;
    }
  }
  bug( "Do_disconnect: *** desc not found ***.", 0 );
  send_to_char( "Descriptor not found!\n\r", ch );
  return;
}


/*
 * Force a level one player to quit.             Gorog
 */
void do_fquit( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MAX_INPUT_LENGTH];

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg1 );
  if( arg1[0] == '\0' )
  {
    send_to_char( "Force whom to quit?\n\r", ch );
    return;
  }
  if( !( victim = get_pc_world( ch, arg1 ) ) )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( victim->level != 1 && !IS_IMMORTAL( ch ) )
  {
    send_to_char( "They are not level one!\n\r", ch );
    return;
  }
  if( get_trust( victim ) >= get_trust( ch ) )
  {
    send_to_char( "&RYou failed, and they saw...&D\n\r", ch );
    ch_printf( victim, "&R%s is attempting to fquit you.&D\n\r", ch->name );
    return;
  }
  set_char_color( AT_IMMORT, victim );
  send_to_char( "You have been forced to quit...\n\r", victim );
  if( victim->fighting )
    stop_fighting( victim, TRUE );
  do_quit( victim, "" );
  ch_printf( ch, "You have forced %s to quit.\n\r", victim->name );
  return;
}

void do_forceclose( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA *d;
  int desc;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Usage: forceclose <descriptor#>\n\r", ch );
    return;
  }

  desc = atoi( arg );
  for( d = first_descriptor; d; d = d->next )
  {
    if( d->descriptor == desc )
    {
      if( d->character && get_trust( d->character ) >= get_trust( ch ) )
      {
        send_to_char( "They might not like that...\n\r", ch );
        return;
      }
      close_socket( d, FALSE );
      send_to_char( "Ok.\n\r", ch );
      return;
    }
  }
  send_to_char( "Not found!\n\r", ch );
  return;
}

void do_pardon( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if( arg1[0] == '\0' || arg2[0] == '\0' )
  {
    send_to_char( "Syntax: pardon <character> <killer|thief|attacker>.\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "attacker" ) )
  {
    if( xIS_SET( victim->act, PLR_ATTACKER ) )
    {
      xREMOVE_BIT( victim->act, PLR_ATTACKER );
      ch_printf( ch, "Attacker flag removed from %s.\n\r", victim->name );
      set_char_color( AT_IMMORT, victim );
      send_to_char( "You are no longer an ATTACKER.\n\r", victim );
    }
    return;
  }
  if( !str_cmp( arg2, "killer" ) )
  {
    if( xIS_SET( victim->act, PLR_KILLER ) )
    {
      xREMOVE_BIT( victim->act, PLR_KILLER );
      ch_printf( ch, "Killer flag removed from %s.\n\r", victim->name );
      set_char_color( AT_IMMORT, victim );
      send_to_char( "You are no longer a KILLER.\n\r", victim );
    }
    return;
  }
  if( !str_cmp( arg2, "thief" ) )
  {
    if( xIS_SET( victim->act, PLR_THIEF ) )
    {
      xREMOVE_BIT( victim->act, PLR_THIEF );
      ch_printf( ch, "Thief flag removed from %s.\n\r", victim->name );
      set_char_color( AT_IMMORT, victim );
      send_to_char( "You are no longer a THIEF.\n\r", victim );
    }
    return;
  }

  if( !str_cmp( arg2, "dead" ) )
  {
    if( xIS_SET( victim->affected_by, AFF_DEAD ) )
    {
      xREMOVE_BIT( victim->affected_by, AFF_DEAD );
      send_to_char( "Your halo dissapiers from above your head.\n\r", ch );
    }
    return;
  }

  send_to_char( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
  return;
}

void echo_to_all( sh_int AT_COLOR, char *argument, sh_int tar )
{
  DESCRIPTOR_DATA *d;

  if( !argument || argument[0] == '\0' )
    return;

  for( d = first_descriptor; d; d = d->next )
  {
    /*
     * Added showing echoes to players who are editing, so they won't
     * miss out on important info like upcoming reboots. --Narn 
     */
    if( d->connected == CON_PLAYING || d->connected == CON_EDITING )
    {
      /*
       * This one is kinda useless except for switched.. 
       */
      if( tar == ECHOTAR_PC && IS_NPC( d->character ) )
        continue;
      else if( tar == ECHOTAR_IMM && !IS_IMMORTAL( d->character ) )
        continue;
      set_char_color( AT_COLOR, d->character );
      send_to_char( argument, d->character );
      send_to_char( "\n\r", d->character );
    }
  }
  return;
}

void do_ech( CHAR_DATA * ch, char *argument )
{
  send_to_char_color( "&YIf you want to echo something, use 'echo'.\n\r", ch );
  return;
}

void do_echo( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  sh_int color;
  int target;
  char *parg;
  char message[MAX_STRING_LENGTH];

  set_char_color( AT_IMMORT, ch );

  if( xIS_SET( ch->act, PLR_NO_EMOTE ) )
  {
    send_to_char( "You can't do that right now.\n\r", ch );
    return;
  }
  if( argument[0] == '\0' )
  {
    send_to_char( "Echo what?\n\r", ch );
    return;
  }

  if( ( color = get_color( argument ) ) )
    argument = one_argument( argument, arg );
  parg = argument;
  argument = one_argument( argument, arg );
  if( !str_cmp( arg, "PC" ) || !str_cmp( arg, "player" ) )
    target = ECHOTAR_PC;
  else if( !str_cmp( arg, "imm" ) )
    target = ECHOTAR_IMM;
  else
  {
    target = ECHOTAR_ALL;
    argument = parg;
  }
  if( !color && ( color = get_color( argument ) ) )
    argument = one_argument( argument, arg );
  if( !color )
    color = AT_IMMORT;
  one_argument( argument, arg );
  sprintf( message, "%s", argument );
  while( strstr( message, "/n" ) && get_trust( ch ) >= LEVEL_ASCENDANT )
  {
    char *ptr;

    ptr = strstr( message, "/n" );

    *ptr = '\n';
    ptr++;
    *ptr = '\r';
  }

  echo_to_all( color, message, target );
}

void echo_to_room( sh_int AT_COLOR, ROOM_INDEX_DATA * room, char *argument )
{
  CHAR_DATA *vic;

  if( room == NULL )
  {
    bug( "NULL ROOM: echo_to_room() %s", argument, 0 );
    return;
  }

  for( vic = room->first_person; vic; vic = vic->next_in_room )
  {
    if( !vic )
    {
      continue;
    }
    set_char_color( AT_COLOR, vic );
    send_to_char( argument, vic );
    send_to_char( "\n\r", vic );
  }
}
void echo_to_area( AREA_DATA * area, char *argument )
{
  CHAR_DATA *vic;

  if( area == NULL )
  {
    bug( "NULL AREA: echo_to_area() %s", argument, 0 );
    return;
  }

  for( vic = first_char; vic; vic = vic->next )
  {
    if( !vic )
      continue;
    if( IS_NPC( vic ) )
      continue;
    if( vic->in_room->area != area )
      continue;
    send_to_char( argument, vic );
    send_to_char( "\n\r", vic );
  }
}

void do_recho( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  sh_int color;
  char message[MAX_STRING_LENGTH];

  set_char_color( AT_IMMORT, ch );

  if( xIS_SET( ch->act, PLR_NO_EMOTE ) )
  {
    send_to_char( "You can't do that right now.\n\r", ch );
    return;
  }
  if( argument[0] == '\0' )
  {
    send_to_char( "Recho what?\n\r", ch );
    return;
  }

  one_argument( argument, arg );

  if( ( color = get_color( argument ) ) )
  {
    argument = one_argument( argument, arg );
    sprintf( message, "%s", argument );
    while( strstr( message, "/n" ) && get_trust( ch ) >= 55 )
    {
      char *ptr;

      ptr = strstr( message, "/n" );

      *ptr = '\n';
      ptr++;
      *ptr = '\r';
    }
    echo_to_room( color, ch->in_room, message );
  }
  else
  {
    sprintf( message, "%s", argument );
    while( strstr( message, "/n" ) && get_trust( ch ) >= 55 )
    {
      char *ptr;

      ptr = strstr( message, "/n" );

      *ptr = '\n';
      ptr++;
      *ptr = '\r';
    }
    echo_to_room( AT_IMMORT, ch->in_room, message );
  }
}

ROOM_INDEX_DATA *find_location( CHAR_DATA * ch, char *arg )
{
  CHAR_DATA *victim;
  OBJ_DATA *obj;

  if( is_number( arg ) )
    return get_room_index( atoi( arg ) );

  if( !str_cmp( arg, "pk" ) ) /* "Goto pk", "at pk", etc */
    return get_room_index( last_pkroom );

  if( ( victim = get_char_world( ch, arg ) ) != NULL )
    return victim->in_room;

  if( ( obj = get_obj_world( ch, arg ) ) != NULL )
    return obj->in_room;

  return NULL;
}

void do_transfer( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *location;
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if( arg1[0] == '\0' )
  {
    send_to_char( "Transfer whom (and where)?\n\r", ch );
    return;
  }
  if( !str_cmp( arg1, "all" ) && get_trust( ch ) >= LEVEL_SUB_IMPLEM )
  {
    for( d = first_descriptor; d; d = d->next )
    {
      if( d->connected == CON_PLAYING
          && d->character != ch && d->character->in_room && d->newstate != 2 && can_see( ch, d->character ) )
      {
        char buf[MAX_STRING_LENGTH];
        sprintf( buf, "%s %s", d->character->name, arg2 );
        do_transfer( ch, buf );
      }
    }
    return;
  }

  /*
   * Thanks to Grodyn for the optional location parameter.
   */

  if( arg2[0] == '\0' )
  {
    location = ch->in_room;
  }
  else
  {
    if( ( location = find_location( ch, arg2 ) ) == NULL )
    {
      send_to_char( "No such location.\n\r", ch );
      return;
    }
    if( room_is_private( location ) && get_trust( ch ) < sysdata.level_override_private )
    {
      send_to_char( "That room is private right now.\n\r", ch );
      return;
    }
    if( room_is_dnd( ch, location ) )
    {
      send_to_char( "That room is marked 'DND' right now.\n\r", ch );
      return;
    }
  }
  if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
/*
	if ( NOT_AUTHED( victim ) )
	{
	   send_to_char( "They might not be in the auth list yet!\n\r", ch);
	   return;
	}
*/
  if( !victim->in_room )
  {
    send_to_char( "They have no physical location!\n\r", ch );
    return;
  }
  if( get_trust( ch ) < get_trust( victim ) && !IS_NPC( victim ) )
  {
    send_to_char( "You better not, that person is more goddly than you.\n\r", ch );
    return;
  }
  /*
   * modification to prevent a low level imm from transferring a 
   */
  /*
   * higher level imm with the DND flag on.  - Gorog             
   */
  if( !IS_NPC( victim ) && get_trust( ch ) < get_trust( victim )
      && victim->desc
      && ( victim->desc->connected == CON_PLAYING
           || victim->desc->connected == CON_EDITING ) && IS_SET( victim->pcdata->flags, PCFLAG_DND ) )
  {
    pager_printf( ch, "Sorry. %s does not wish to be disturbed.\n\r", victim->name );
    pager_printf( victim, "Your DND flag just foiled %s's transfer command.\n\r", ch->name );
    return;
  }
  /*
   * end of modification                                         
   */

  if( victim->fighting )
    stop_fighting( victim, TRUE );
  act( AT_MAGIC, "$n disappears in a cloud of swirling colors.", victim, NULL, NULL, TO_ROOM );
  victim->retran = victim->in_room->vnum;
  char_from_room( victim );
  char_to_room( victim, location );
  act( AT_MAGIC, "$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM );
  if( ch != victim )
    act( AT_IMMORT, "$n has transferred you.", ch, NULL, victim, TO_VICT );
  do_look( victim, "auto" );
  if( !IS_IMMORTAL( victim ) && !IS_NPC( victim ) && !in_hard_range( victim, location->area ) )
    act( AT_DANGER, "Warning:  this player's level is not within the area's level range.", ch, NULL, NULL, TO_CHAR );
}

void do_retran( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Retransfer whom?\n\r", ch );
    return;
  }
  if( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  sprintf( buf, "'%s' %d", victim->name, victim->retran );
  do_transfer( ch, buf );
  return;
}

void do_regoto( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];

  sprintf( buf, "%d", ch->regoto );
  do_goto( ch, buf );
  return;
}

/*  Added do_at and do_atobj to reduce lag associated with at
 *  --Shaddai
 */
void do_at( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *location = NULL;
  ROOM_INDEX_DATA *original;
  CHAR_DATA *wch = NULL, *victim;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( arg[0] == '\0' || argument[0] == '\0' )
  {
    send_to_char( "At where what?\n\r", ch );
    return;
  }
  if( is_number( arg ) )
    location = get_room_index( atoi( arg ) );
  else if( !str_cmp( arg, "pk" ) )
    location = get_room_index( last_pkroom );
  else if( ( wch = get_char_world( ch, arg ) ) == NULL || wch->in_room == NULL )
  {
    send_to_char( "No such mobile or player in existance.\n\r", ch );
    return;
  }
  if( !location && wch )
    location = wch->in_room;

  if( !location )
  {
    send_to_char( "No such location exists.\n\r", ch );
    return;
  }

  /*
   * The following mod is used to prevent players from using the 
   */
  /*
   * at command on a higher level immortal who has a DND flag    
   */
  if( wch && !IS_NPC( wch ) && IS_SET( wch->pcdata->flags, PCFLAG_DND ) && get_trust( ch ) < get_trust( wch ) )
  {
    pager_printf( ch, "Sorry. %s does not wish to be disturbed.\n\r", wch->name );
    pager_printf( wch, "Your DND flag just foiled %s's at command.\n\r", ch->name );
    return;
  }
  /*
   * End of modification  -- Gorog 
   */


  if( room_is_private( location ) )
  {
    if( get_trust( ch ) < sysdata.level_override_private )
    {
      send_to_char( "That room is private right now.\n\r", ch );
      return;
    }
    else
      send_to_char( "Overriding private flag!\n\r", ch );
  }

  if( ( victim = room_is_dnd( ch, location ) ) )
  {
    pager_printf( ch, "That room is \"do not disturb\" right now.\n\r" );
    pager_printf( victim, "Your DND flag just foiled %s's atmob command\n\r", ch->name );
    return;
  }

  set_char_color( AT_PLAIN, ch );
  original = ch->in_room;
  char_from_room( ch );
  char_to_room( ch, location );
  interpret( ch, argument );

  if( !char_died( ch ) )
  {
    char_from_room( ch );
    char_to_room( ch, original );
  }
  return;
}

void do_atobj( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *location;
  ROOM_INDEX_DATA *original;
  OBJ_DATA *obj;
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( arg[0] == '\0' || argument[0] == '\0' )
  {
    send_to_char( "At where what?\n\r", ch );
    return;
  }

  if( ( obj = get_obj_world( ch, arg ) ) == NULL || !obj->in_room )
  {
    send_to_char( "No such object in existance.\n\r", ch );
    return;
  }
  location = obj->in_room;
  if( room_is_private( location ) )
  {
    if( get_trust( ch ) < sysdata.level_override_private )
    {
      send_to_char( "That room is private right now.\n\r", ch );
      return;
    }
    else
      send_to_char( "Overriding private flag!\n\r", ch );
  }

  if( ( victim = room_is_dnd( ch, location ) ) )
  {
    pager_printf( ch, "That room is \"do not disturb\" right now.\n\r" );
    pager_printf( victim, "Your DND flag just foiled %s's atobj command\n\r", ch->name );
    return;
  }

  set_char_color( AT_PLAIN, ch );
  original = ch->in_room;
  char_from_room( ch );
  char_to_room( ch, location );
  interpret( ch, argument );

  if( !char_died( ch ) )
  {
    char_from_room( ch );
    char_to_room( ch, original );
  }
  return;
}

void do_rat( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *location;
  ROOM_INDEX_DATA *original;
  int Start, End, vnum;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if( arg1[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0' )
  {
    send_to_char( "Syntax: rat <start> <end> <command>\n\r", ch );
    return;
  }

  Start = atoi( arg1 );
  End = atoi( arg2 );
  if( Start < 1 || End < Start || Start > End || Start == End || End > MAX_VNUMS )
  {
    send_to_char( "Invalid range.\n\r", ch );
    return;
  }
  if( !str_cmp( argument, "quit" ) )
  {
    send_to_char( "I don't think so!\n\r", ch );
    return;
  }

  original = ch->in_room;
  for( vnum = Start; vnum <= End; vnum++ )
  {
    if( ( location = get_room_index( vnum ) ) == NULL )
      continue;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );
  }

  char_from_room( ch );
  char_to_room( ch, original );
  send_to_char( "Done.\n\r", ch );
  return;
}

void do_rstat( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  char *sect;
  ROOM_INDEX_DATA *location;
  OBJ_DATA *obj;
  CHAR_DATA *rch;
  EXIT_DATA *pexit;
  AFFECT_DATA *paf;
  int cnt;
  static char *dir_text[] = { "n", "e", "s", "w", "u", "d", "ne", "nw", "se", "sw", "?" };

  one_argument( argument, arg );
  if( !str_cmp( arg, "ex" ) || !str_cmp( arg, "exits" ) )
  {
    location = ch->in_room;

    if( !xIS_SET( location->room_flags, ROOM_PROTOTYPE ) && get_trust( ch ) < sysdata.level_modify_proto )
    {
      send_to_char( "You don't have the power to do that.\n\r", ch );
      return;
    }

    ch_printf_color( ch, "&cExits for room '&W%s&c'  Vnum &W%d\n\r", location->name, location->vnum );
    for( cnt = 0, pexit = location->first_exit; pexit; pexit = pexit->next )
      ch_printf_color( ch,
                       "&W%2d) &w%2s to %-5d  &cKey: &w%d  &cFlags: &w%d  &cKeywords: '&w%s&c'\n\r     Exdesc: &w%s     &cBack link: &w%d  &cVnum: &w%d  &cDistance: &w%d  &cPulltype: &w%s  &cPull: &w%d\n\r",
                       ++cnt,
                       dir_text[pexit->vdir],
                       pexit->to_room ? pexit->to_room->vnum : 0,
                       pexit->key,
                       pexit->exit_info,
                       pexit->keyword,
                       pexit->description[0] != '\0'
                       ? pexit->description : "(none).\n\r",
                       pexit->rexit ? pexit->rexit->vnum : 0,
                       pexit->rvnum, pexit->distance, pull_type_name( pexit->pulltype ), pexit->pull );
    return;
  }
  location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
  if( !location )
  {
    send_to_char( "No such location.\n\r", ch );
    return;
  }

/*
	if (!xIS_SET(location->room_flags, ROOM_PROTOTYPE)
	    && get_trust( ch ) < sysdata.level_modify_proto )
	{
		send_to_char( "You don't have the power to do that.\n\r", ch );
		return ;
	}
*/
  if( ch->in_room != location && room_is_private( location ) )
  {
    if( get_trust( ch ) < sysdata.level_override_private )
    {
      send_to_char( "That room is private right now.\n\r", ch );
      return;
    }
    else
      send_to_char( "Overriding private flag!\n\r", ch );
  }

  ch_printf_color( ch, "&cName: &w%s\n\r&cArea: &w%s  &cFilename: &w%s\n\r",
                   location->name,
                   location->area ? location->area->name : "None????",
                   location->area ? location->area->filename : "None????" );

  switch ( ch->in_room->sector_type )
  {
    default:
      sect = "BUG: NOTIFY GOKU";
      break;
    case SECT_INSIDE:
      sect = "Inside";
      break;
    case SECT_CITY:
      sect = "City";
      break;
    case SECT_FIELD:
      sect = "Field";
      break;
    case SECT_FOREST:
      sect = "Forest";
      break;
    case SECT_HILLS:
      sect = "Hills";
      break;
    case SECT_MOUNTAIN:
      sect = "Mountains";
      break;
    case SECT_WATER_SWIM:
      sect = "Swim";
      break;
    case SECT_WATER_NOSWIM:
      sect = "Noswim";
      break;
    case SECT_UNDERWATER:
      sect = "Underwater";
      break;
    case SECT_AIR:
      sect = "Air";
      break;
    case SECT_DESERT:
      sect = "Desert";
      break;
    case SECT_OCEANFLOOR:
      sect = "Oceanfloor";
      break;
    case SECT_UNDERGROUND:
      sect = "Underground";
      break;
    case SECT_LAVA:
      sect = "Lava";
      break;
    case SECT_SWAMP:
      sect = "Swamp";
      break;
    case SECT_SPACE:
      sect = "Space";
      break;
  }

  ch_printf_color( ch, "&cVnum: &w%d   &cSector: &w%d (%s)   &cLight: &w%d",
                   location->vnum, location->sector_type, sect, location->light );
  if( location->tunnel > 0 )
    ch_printf_color( ch, "   &cTunnel: &W%d", location->tunnel );
  send_to_char( "\n\r", ch );
  if( location->tele_delay > 0 || location->tele_vnum > 0 )
    ch_printf_color( ch, "&cTeleDelay: &R%d   &cTeleVnum: &R%d\n\r", location->tele_delay, location->tele_vnum );
  ch_printf_color( ch, "&cRoom flags: &w%s\n\r", ext_flag_string( &location->room_flags, r_flags ) );
  ch_printf_color( ch, "&cDescription:\n\r&w%s", location->description );
  if( location->first_extradesc )
  {
    EXTRA_DESCR_DATA *ed;

    send_to_char_color( "&cExtra description keywords: &w'", ch );
    for( ed = location->first_extradesc; ed; ed = ed->next )
    {
      send_to_char( ed->keyword, ch );
      if( ed->next )
        send_to_char( " ", ch );
    }
    send_to_char( "'\n\r", ch );
  }
  for( paf = location->first_affect; paf; paf = paf->next )
    ch_printf_color( ch, "&cAffect: &w%s &cby &w%d.\n\r", affect_loc_name( paf->location ), paf->modifier );

  send_to_char_color( "&cCharacters: &w", ch );
  for( rch = location->first_person; rch; rch = rch->next_in_room )
  {
    if( can_see( ch, rch ) )
    {
      send_to_char( " ", ch );
      one_argument( rch->name, buf );
      send_to_char( buf, ch );
    }
  }

  send_to_char_color( "\n\r&cObjects:    &w", ch );
  for( obj = location->first_content; obj; obj = obj->next_content )
  {
    send_to_char( " ", ch );
    one_argument( obj->name, buf );
    send_to_char( buf, ch );
  }
  send_to_char( "\n\r", ch );

  if( location->first_exit )
    send_to_char_color( "&c------------------- &wEXITS &c-------------------\n\r", ch );
  for( cnt = 0, pexit = location->first_exit; pexit; pexit = pexit->next )
    ch_printf( ch,
               "%2d) %-2s to %-5d.  Key: %d  Flags: %d  Keywords: %s.\n\r",
               ++cnt,
               dir_text[pexit->vdir],
               pexit->to_room ? pexit->to_room->vnum : 0,
               pexit->key, pexit->exit_info, pexit->keyword[0] != '\0' ? pexit->keyword : "(none)" );
  return;
}

/* Face-lift by Demora */
void do_ostat( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  AFFECT_DATA *paf;
  OBJ_DATA *obj;
  double plMod;

  set_char_color( AT_CYAN, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Ostat what?\n\r", ch );
    return;
  }
  if( arg[0] != '\'' && arg[0] != '"' && strlen( argument ) > strlen( arg ) )
    strcpy( arg, argument );

  if( ( obj = get_obj_world( ch, arg ) ) == NULL )
  {
    send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
    return;
  }

  if( !IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) && get_trust( ch ) < sysdata.level_modify_proto )
  {
    send_to_char( "You don't have the power to do that.\n\r", ch );
    return;
  }
  ch_printf( ch, "\n\r" );
  ch_printf_color( ch, "&cName: &C%s\n\r", obj->name );
  ch_printf_color( ch, "&cVnum: &w%d  ", obj->pIndexData->vnum );
  ch_printf_color( ch, "&cType: &w%s  ", item_type_name( obj ) );
  ch_printf_color( ch, "&cCount:  &w%d  ", obj->pIndexData->count );
  ch_printf_color( ch, "&cGcount: &w%d\n\r", obj->count );
  ch_printf_color( ch, "&cSerial#: &w%d  ", obj->serial );
  ch_printf_color( ch, "&cTopIdxSerial#: &w%d  ", obj->pIndexData->serial );
  ch_printf_color( ch, "&cTopSerial#: &w%d\n\r", cur_obj_serial );
  ch_printf_color( ch, "&cShort description: &C%s\n\r", obj->short_descr );
  ch_printf_color( ch, "&cLong description : &C%s\n\r", obj->description );
  if( obj->action_desc[0] != '\0' )
    ch_printf_color( ch, "&cAction description: &w%s\n\r", obj->action_desc );
  ch_printf_color( ch, "&cWear flags : &w%s\n\r", flag_string( obj->wear_flags, w_flags ) );
  ch_printf_color( ch, "&cExtra flags: &w%s\n\r", ext_flag_string( &obj->extra_flags, o_flags ) );
  ch_printf_color( ch, "&cMagic flags: &w%s\n\r", magic_bit_name( obj->magic_flags ) );
  ch_printf_color( ch, "&cNumber: &w%d/%d   ", 1, get_obj_number( obj ) );
  ch_printf_color( ch, "&cWeight: &w%s/%s   ", num_punct( obj->weight ), num_punct( get_obj_weight( obj ) ) );
  ch_printf_color( ch, "&cLayers: &w%d   ", obj->pIndexData->layers );
  ch_printf_color( ch, "&cWear_loc: &w%d\n\r", obj->wear_loc );
  ch_printf_color( ch, "&cCost: &Y%s  ", num_punct( obj->cost ) );
  ch_printf_color( ch, "&cRent: &w%s  ", num_punct( obj->pIndexData->rent ) );
  send_to_char_color( "&cTimer: ", ch );
  if( obj->timer > 0 )
    ch_printf_color( ch, "&R%d  ", obj->timer );
  else
    ch_printf_color( ch, "&w%d  ", obj->timer );
  ch_printf_color( ch, "&cLevel: &P%s\n\r", num_punct_ld( obj->level ) );
  ch_printf_color( ch, "&cIn room: &w%d  ", obj->in_room == NULL ? 0 : obj->in_room->vnum );
  ch_printf_color( ch, "&cIn object: &w%s  ", obj->in_obj == NULL ? "(none)" : obj->in_obj->short_descr );
  ch_printf_color( ch, "&cCarried by: &C%s\n\r", obj->carried_by == NULL ? "(none)" : obj->carried_by->name );
  ch_printf_color( ch, "&cIndex Values : &w%d %d %d %d %d %d.\n\r",
                   obj->pIndexData->value[0], obj->pIndexData->value[1],
                   obj->pIndexData->value[2], obj->pIndexData->value[3],
                   obj->pIndexData->value[4], obj->pIndexData->value[5] );
  ch_printf_color( ch, "&cObject Values: &w%d %d %d %d %d %d.\n\r",
                   obj->value[0], obj->value[1], obj->value[2], obj->value[3], obj->value[4], obj->value[5] );
  if( obj->item_type == ITEM_ARMOR )
  {
    plMod = obj->value[3];
    plMod = plMod / 10000;
    ch_printf_color( ch, "&W *Object Value 3 is giving a %f%% bonus to PL gains.\n\r", plMod * 100 );
  }
  if( obj->pIndexData->first_extradesc )
  {
    EXTRA_DESCR_DATA *ed;
    send_to_char( "Primary description keywords:   '", ch );
    for( ed = obj->pIndexData->first_extradesc; ed; ed = ed->next )
    {
      send_to_char( ed->keyword, ch );
      if( ed->next )
        send_to_char( " ", ch );
    }
    send_to_char( "'.\n\r", ch );
  }
  if( obj->first_extradesc )
  {
    EXTRA_DESCR_DATA *ed;
    send_to_char( "Secondary description keywords: '", ch );
    for( ed = obj->first_extradesc; ed; ed = ed->next )
    {
      send_to_char( ed->keyword, ch );
      if( ed->next )
        send_to_char( " ", ch );
    }
    send_to_char( "'.\n\r", ch );
  }
  for( paf = obj->first_affect; paf; paf = paf->next )
    ch_printf_color( ch, "&cAffects &w%s &cby &w%d. (extra)\n\r", affect_loc_name( paf->location ), paf->modifier );
  for( paf = obj->pIndexData->first_affect; paf; paf = paf->next )
    ch_printf_color( ch, "&cAffects &w%s &cby &w%d.\n\r", affect_loc_name( paf->location ), paf->modifier );
  return;
}


void do_mstat( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char hpbuf[MAX_STRING_LENGTH];
  char mnbuf[MAX_STRING_LENGTH];
  char mvbuf[MAX_STRING_LENGTH];
  //    char bdbuf[MAX_STRING_LENGTH];
  AFFECT_DATA *paf;
  CHAR_DATA *victim;
  SKILLTYPE *skill;
  int x;

  set_pager_color( AT_CYAN, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_pager( "Mstat whom?\n\r", ch );
    return;
  }
  if( arg[0] != '\'' && arg[0] != '"' && strlen( argument ) > strlen( arg ) )
    strcpy( arg, argument );

  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_pager( "They aren't here.\n\r", ch );
    return;
  }
  if( get_trust( ch ) < get_trust( victim ) && !IS_NPC( victim ) )
  {
    set_pager_color( AT_IMMORT, ch );
    send_to_pager( "Their godly glow prevents you from getting a good look.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) && get_trust( ch ) < LEVEL_GREATER && xIS_SET( victim->act, ACT_STATSHIELD ) )
  {
    set_pager_color( AT_IMMORT, ch );
    send_to_pager( "Their godly glow prevents you from getting a good look.\n\r", ch );
    return;
  }

  if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_DEMI /*LEVEL_SUB_IMPLEM */  )
  {
    send_to_char( "You don't have the power to do that.\n\r", ch );
    return;
  }


  if( IS_NPC( victim ) && !xIS_SET( victim->act, ACT_PROTOTYPE ) && get_trust( ch ) < sysdata.level_modify_proto )
  {
    send_to_char( "You don't have the power to do that.\n\r", ch );
    return;
  }

  pager_printf_color( ch, "\n\r&c%s: &C%-20s", IS_NPC( victim ) ? "Mobile name" : "Name", victim->name );
  if( !IS_NPC( victim ) )
    pager_printf_color( ch, "&cStatus : &w%-10s", CAN_PKILL( victim ) ? "Deadly" :
                        IS_PKILL( victim ) ? "Pre-Deadly" : "Non-Deadly" );
  if( !IS_NPC( victim ) && victim->pcdata->clan )
    pager_printf_color( ch, "   &c%s: &w%s",
                        victim->pcdata->clan->clan_type == CLAN_ORDER ? "Order" :
                        victim->pcdata->clan->clan_type == CLAN_GUILD ? "Guild" : "Clan", victim->pcdata->clan->name );
  send_to_pager( "\n\r", ch );
  if( get_trust( ch ) >= LEVEL_GOD && !IS_NPC( victim ) && victim->desc )
    pager_printf_color( ch, "&cUser: &w%s@%s   Descriptor: %d  &cTrust: &w%d  &cAuthBy: &w%s\n\r",
                        victim->desc->user, victim->desc->host, victim->desc->descriptor,
                        0 /*victim->trust */ , victim->pcdata->authed_by[0] != '\0' ?
                        victim->pcdata->authed_by : "(unknown)" );
  if( !IS_NPC( victim ) && victim->pcdata->release_date != 0 )
    pager_printf_color( ch, "&cHelled until %24.24s by %s.\n\r",
                        ctime( &victim->pcdata->release_date ), victim->pcdata->helled_by );
  pager_printf_color( ch, "&cVnum: &w%-5d    &cSex: &w%-6s    &cRoom: &w%-5d    &cCount: &w%d   &cKilled: &w%d\n\r",
                      IS_NPC( victim ) ? victim->pIndexData->vnum : 0,
                      victim->sex == SEX_MALE ? "male" :
                      victim->sex == SEX_FEMALE ? "female" : "neutral",
                      victim->in_room == NULL ? 0 : victim->in_room->vnum,
                      IS_NPC( victim ) ? victim->pIndexData->count : 1,
                      IS_NPC( victim ) ? victim->pIndexData->killed : victim->pcdata->mdeaths + victim->pcdata->pdeaths );
  pager_printf_color( ch,
                      "&cStr: &C%2d&c )( Int: &C%2d&c )( Wis: &C0&c )( Dex: &C%2d&c )( Con: &C%2d&c )( Cha: &C0&c )( Lck: &C%2d&c\n\r",
                      get_curr_str( victim ), get_curr_int( victim ), get_curr_dex( victim ), get_curr_con( victim ),
                      get_curr_lck( victim ) );
  pager_printf_color( ch, "&cLevel   : &P%-2d              ", victim->level );
  pager_printf_color( ch, "&cClass  : &w%-2.2d/%-10s   &cRace      : &w%-2.2d/%-10s\n\r",
                      victim->class,
                      IS_NPC( victim ) ? victim->class < MAX_NPC_CLASS && victim->class >= 0 ?
                      npc_class[victim->class] : "unknown" : victim->class < MAX_PC_CLASS &&
                      class_table[victim->class]->who_name &&
                      class_table[victim->class]->who_name[0] != '\0' ?
                      class_table[victim->class]->who_name : "unknown",
                      victim->race,
                      IS_NPC( victim ) ? victim->race < MAX_NPC_RACE && victim->race >= 0 ?
                      npc_race[victim->race] : "unknown" : victim->race < MAX_PC_RACE &&
                      race_table[victim->race]->race_name &&
                      race_table[victim->race]->race_name[0] != '\0' ?
                      race_table[victim->race]->race_name : "unknown", npc_race[victim->race], victim->position );
  sprintf( hpbuf, "%d/%d", victim->hit, victim->max_hit );
  sprintf( mnbuf, "%d/%d", victim->mana, victim->max_mana );
  sprintf( mvbuf, "%d/%d", get_armor( victim ), get_maxarmor( victim ) );
  pager_printf_color( ch, "&cHps     : &w%-12s    &cMana   : &w%-12s    &cArmor     : &w%-12s\n\r", hpbuf, mnbuf, mvbuf );
  pager_printf_color( ch, "&cHitroll : &CN/A             &cAlign  : &w%-5d           &cDamage %% : &w%d\n\r",
                      victim->alignment, GET_AC( victim ) );
  pager_printf_color( ch, "&cDamroll : &C%-5d           &cWimpy  : &w%-5d           &cPosition  : &w%d\n\r",
                      GET_DAMROLL( victim ), victim->wimpy, victim->position );
  pager_printf_color( ch, "&cFighting: &w%-13s   &cMaster : &w%-13s   &cLeader    : &w%s\n\r",
                      victim->fighting ? victim->fighting->who->name : "(none)",
                      victim->master ? victim->master->name : "(none)", victim->leader ? victim->leader->name : "(none)" );
  if( IS_NPC( victim ) )
    pager_printf_color( ch, "&cHating  : &w%-13s   &cHunting: &w%-13s   &cFearing   : &w%s\n\r",
                        victim->hating ? victim->hating->name : "(none)",
                        victim->hunting ? victim->hunting->name : "(none)",
                        victim->fearing ? victim->fearing->name : "(none)" );
  else
    pager_printf_color( ch, "&cDeity   : &w%-13s&w   &cFavor  : &w%-5d           &cGlory     : &w%-d (%d)\n\r",
                        victim->pcdata->deity ? victim->pcdata->deity->name : "(none)",
                        victim->pcdata->favor, victim->pcdata->quest_curr, victim->pcdata->quest_accum );
  if( IS_NPC( victim ) )
    pager_printf_color( ch, "&cMob hitdie : &C%dd%d+%d    &cMob damdie : &C%dd%d+%d          &cNumAttacks(extra) : &C%d\n\r",
                        victim->pIndexData->hitnodice,
                        victim->pIndexData->hitsizedice,
                        victim->pIndexData->hitplus,
                        victim->pIndexData->damnodice,
                        victim->pIndexData->damsizedice, victim->pIndexData->damplus, victim->numattacks );
  pager_printf_color( ch, "&cMentalState: &w%-3d   &cEmotionalState: &w%-3d   ",
                      victim->mental_state, victim->emotional_state );
  if( !IS_NPC( victim ) )
    pager_printf_color( ch, "&cThirst: &w%d   &cFull: &w%d   &cDrunk: &w%d\n\r",
                        victim->pcdata->condition[COND_THIRST],
                        victim->pcdata->condition[COND_FULL], victim->pcdata->condition[COND_DRUNK] );
  else
    send_to_pager( "\n\r", ch );
  pager_printf_color( ch, "&cSave versus: &w%d %d %d %d %d       &cItems: &w(%d/%d)  &cWeight &w(%s/%s)\n\r",
                      victim->saving_poison_death,
                      victim->saving_wand,
                      victim->saving_para_petri,
                      victim->saving_breath,
                      victim->saving_spell_staff,
                      victim->carry_number,
                      can_carry_n( victim ), num_punct( victim->carry_weight ), num_punct( can_carry_w( victim ) ) );
  pager_printf_color( ch, "&cYear: &w%-5d  &cSecs: &w%d  &cTimer: &w%d  &cZeni: &Y%s\n\r",
                      get_age( victim ), ( int )victim->played, victim->timer, num_punct( victim->gold ) );
  if( get_timer( victim, TIMER_PKILLED ) )
    pager_printf_color( ch, "&cTimerPkilled:  &R%d\n\r", get_timer( victim, TIMER_PKILLED ) );
  if( get_timer( victim, TIMER_RECENTFIGHT ) )
    pager_printf_color( ch, "&cTimerRecentfight:  &R%d\n\r", get_timer( victim, TIMER_RECENTFIGHT ) );
  if( get_timer( victim, TIMER_ASUPRESSED ) )
    pager_printf_color( ch, "&cTimerAsupressed:  &R%d\n\r", get_timer( victim, TIMER_ASUPRESSED ) );
  if( IS_NPC( victim ) )
    pager_printf_color( ch, "&cAct Flags  : &w%s\n\r", ext_flag_string( &victim->act, act_flags ) );
  else
  {
//    pager_printf_color( ch, "&cPlayerFlags: &w%s\n\r", ext_flag_string( &victim->act, plr_flags ) );
    pager_printf_color( ch, "&cPcflags    : &w%s\n\r", flag_string( victim->pcdata->flags, pc_flags ) );
    if( ch->level >= 63 )
    {
      if( xIS_SET( victim->act, PLR_NOSCREAM ) )
        pager_printf( ch, "&cNoscream: &wYes.\n\r" );
      else
        pager_printf( ch, "&cNoscream: &wNo.\n\r" );
    }
    if( victim->pcdata->nuisance )
    {
      pager_printf_color( ch, "&RNuisance   &cStage: (&R%d&c/%d)  Power:  &w%d  &cTime:  &w%s.\n\r",
                          victim->pcdata->nuisance->flags, MAX_NUISANCE_STAGE, victim->pcdata->nuisance->power,
                          ctime( &victim->pcdata->nuisance->time ) );
    }
    if( victim->pcdata && victim->pcdata->nextHBTCDate > 0 )
    {
      pager_printf_color( ch, "&cNextHBTCDate: &W%24.24s&w\n\r", ctime( &victim->pcdata->nextHBTCDate ) );
    }
    if( victim->pcdata && victim->pcdata->HBTCTimeLeft > 0 )
    {
      pager_printf_color( ch, "&cHBTCTimeLeft: &W%d&w\n\r", victim->pcdata->HBTCTimeLeft );
    }
    if( victim->pcdata && victim->pcdata->HBTCPartner != '\0' )
    {
      pager_printf_color( ch, "&cHBTCPartner: &W%s&w\n\r", victim->pcdata->HBTCPartner );
    }
  }
  if( victim->morph )
  {
    if( victim->morph->morph )
      pager_printf_color( ch, "&cMorphed as : (&C%d&c) &C%s    &cTimer: &C%d\n\r",
                          victim->morph->morph->vnum, victim->morph->morph->short_desc, victim->morph->timer );
    else
      pager_printf_color( ch, "&cMorphed as: Morph was deleted.\n\r" );
  }
  pager_printf_color( ch, "&cAffected by: &C%s\n\r", affect_bit_name( &victim->affected_by ) );
  pager_printf_color( ch, "&cSpeaks: &w%d   &cSpeaking: &w%d   &cPowerLevel: &w%s",
                      victim->speaks, victim->speaking,
                      ( get_trust( ch ) < 59 && !IS_NPC( victim ) ) ? "0" : num_punct_ld( victim->exp ) );
  if( IS_NPC( victim ) )
    pager_printf( ch, "&c/&w%s", num_punct_ld( victim->pl ) );
  if( !IS_NPC( victim ) && victim->wait )
    pager_printf_color( ch, "   &cWaitState: &R%d\n\r", victim->wait / 12 );
  else
    send_to_pager( "\n\r", ch );
  send_to_pager_color( "&cLanguages  : &w", ch );
  for( x = 0; lang_array[x] != LANG_UNKNOWN; x++ )
    if( knows_language( victim, lang_array[x], victim ) || ( IS_NPC( victim ) && victim->speaks == 0 ) )
    {
      if( IS_SET( lang_array[x], victim->speaking ) || ( IS_NPC( victim ) && !victim->speaking ) )
        set_pager_color( AT_RED, ch );
      send_to_pager( lang_names[x], ch );
      send_to_pager( " ", ch );
      set_pager_color( AT_PLAIN, ch );
    }
    else if( IS_SET( lang_array[x], victim->speaking ) || ( IS_NPC( victim ) && !victim->speaking ) )
    {
      set_pager_color( AT_PINK, ch );
      send_to_pager( lang_names[x], ch );
      send_to_pager( " ", ch );
      set_pager_color( AT_PLAIN, ch );
    }
  send_to_pager( "\n\r", ch );
  if( victim->pcdata && victim->pcdata->bestowments && victim->pcdata->bestowments[0] != '\0' )
    pager_printf_color( ch, "&cBestowments: &w%s\n\r", victim->pcdata->bestowments );
  if( IS_NPC( victim ) )
    pager_printf_color( ch, "&cShortdesc  : &w%s\n\r&cLongdesc   : &w%s\r\n",
                        victim->short_descr[0] != '\0' ? victim->short_descr : "(none set)",
                        victim->long_descr[0] != '\0' ? victim->long_descr : "(none set)\n\r" );
  else
  {
    if( victim->short_descr[0] != '\0' )
      pager_printf_color( ch, "&cShortdesc  : &w%s\n\r", victim->short_descr );
    if( victim->long_descr[0] != '\0' )
      pager_printf_color( ch, "&cLongdesc   : &w%s\n\r", victim->long_descr );
  }
  if( IS_NPC( victim ) && victim->spec_fun )
    pager_printf_color( ch, "&cMobile has spec fun: &w%s\n\r", lookup_spec( victim->spec_fun ) );
  if( IS_NPC( victim ) )
    pager_printf_color( ch, "&cBody Parts : &w%s\n\r", flag_string( victim->xflags, part_flags ) );
  if( victim->resistant > 0 )
    pager_printf_color( ch, "&cResistant  : &w%s\n\r", flag_string( victim->resistant, ris_flags ) );
  if( victim->immune > 0 )
    pager_printf_color( ch, "&cImmune     : &w%s\n\r", flag_string( victim->immune, ris_flags ) );
  if( victim->susceptible > 0 )
    pager_printf_color( ch, "&cSusceptible: &w%s\n\r", flag_string( victim->susceptible, ris_flags ) );
  if( IS_NPC( victim ) )
  {
    pager_printf_color( ch, "&cAttacks    : &w%s\n\r", ext_flag_string( &victim->attacks, attack_flags ) );
    pager_printf_color( ch, "&cDefenses   : &w%s\n\r", ext_flag_string( &victim->defenses, defense_flags ) );
  }
  for( paf = victim->first_affect; paf; paf = paf->next )
  {
    if( ( skill = get_skilltype( paf->type ) ) != NULL )
      pager_printf_color( ch,
                          "&c%s: &w'%s' mods %s by %d for %d rnds with bits %s.",
                          skill_tname[skill->type],
                          skill->name,
                          affect_loc_name( paf->location ),
                          paf->modifier, paf->duration, affect_bit_name( &paf->bitvector ) );
    send_to_char( "\n\r", ch );
  }
  ch_printf( ch, "\n\r&cFusions: &w%d\n\r", victim->fusions );
  ch_printf( ch, "&cFusionflags: &w%s\n\r", flag_string( victim->fusionflags, fusion_flags ) );
  ch_printf( ch, "&cFused with: " );
  int a = 0;
  for( a = 0; a < victim->fusions; a++ )
  {
    ch_printf( ch, "&w%s ", victim->fused[a] );
  }
  ch_printf( ch, "\n\r" );
  if( is_android( victim ) || is_superandroid( victim ) )
  {
    ch_printf( ch, "&cAndroid Stored Cores Pl: &w%s\n\r", num_punct_ld( victim->corespl ) );
    ch_printf( ch, "&cAbsorbed FM type core: &w%s\n\r", ( ( victim->fm_core ) ? "Yes" : "No" ) );
    ch_printf( ch, "&cAbsorbed E type core: &w%s\n\r", ( ( victim->e_core ) ? "Yes" : "No" ) );
    ch_printf( ch, "&cAbsorbed H type core: &w%s\n\r", ( ( victim->h_core ) ? "Yes" : "No" ) );
  }
  if( victim->pcdata && ch->level >= 63 )
  {
    ch_printf( ch, "&cUpdateL: &w%d\n\r", victim->pcdata->upgradeL );
    if( IS_ZERO(victim) || IS_DOMINIC(victim) || IS_STARWIND( victim ) || IS_DOMINIC2( victim ) )
      ch_printf( ch, "&cWeighted Gains: &w2.00\n\r", weightedClothingPlMod( victim ) );
    else
      ch_printf( ch, "&cWeighted Gains: &w%f%%\n\r", weightedClothingPlMod( victim ) );
  }

  if(IS_RANKED(victim)) 
    ch_printf(ch, "Racial Rank: %s(%d)\r\n", get_ch_rank(victim), victim->rank);

  ch_printf( ch, "\n\r" );
  return;
}

void do_mfind( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  MOB_INDEX_DATA *pMobIndex;
  int hash;
  int nMatch;
  bool fAll;

  set_pager_color( AT_PLAIN, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Mfind whom?\n\r", ch );
    return;
  }

  fAll = !str_cmp( arg, "all" );
  nMatch = 0;

  /*
   * This goes through all the hash entry points (1024), and is therefore
   * much faster, though you won't get your vnums in order... oh well. :)
   *
   * Tests show that Furey's method will usually loop 32,000 times, calling
   * get_mob_index()... which loops itself, an average of 1-2 times...
   * So theoretically, the above routine may loop well over 40,000 times,
   * and my routine bellow will loop for as many index_mobiles are on
   * your mud... likely under 3000 times.
   * -Thoric
   */
  for( hash = 0; hash < MAX_KEY_HASH; hash++ )
    for( pMobIndex = mob_index_hash[hash]; pMobIndex; pMobIndex = pMobIndex->next )
      if( fAll || nifty_is_name( arg, pMobIndex->player_name ) )
      {
        nMatch++;
        pager_printf( ch, "[%5d] %s\n\r", pMobIndex->vnum, capitalize( pMobIndex->short_descr ) );
      }

  if( nMatch )
    pager_printf( ch, "Number of matches: %d\n", nMatch );
  else
    send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
  return;
}

void do_ofind( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_INDEX_DATA *pObjIndex;
  int hash;
  int nMatch;
  bool fAll;

  set_pager_color( AT_PLAIN, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Ofind what?\n\r", ch );
    return;
  }

  fAll = !str_cmp( arg, "all" );
  nMatch = 0;

  /*
   * This goes through all the hash entry points (1024), and is therefore
   * much faster, though you won't get your vnums in order... oh well. :)
   *
   * Tests show that Furey's method will usually loop 32,000 times, calling
   * get_obj_index()... which loops itself, an average of 2-3 times...
   * So theoretically, the above routine may loop well over 50,000 times,
   * and my routine bellow will loop for as many index_objects are on
   * your mud... likely under 3000 times.
   * -Thoric
   */
  for( hash = 0; hash < MAX_KEY_HASH; hash++ )
    for( pObjIndex = obj_index_hash[hash]; pObjIndex; pObjIndex = pObjIndex->next )
      if( fAll || nifty_is_name( arg, pObjIndex->name ) )
      {
        nMatch++;
        pager_printf( ch, "[%5d] %s\n\r", pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
      }

  if( nMatch )
    pager_printf( ch, "Number of matches: %d\n", nMatch );
  else
    send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
  return;
}

/**************************************
 * Find object matching a certain type*
 *           By: Karn (DBNA)          *
 **************************************/
void do_ofindtype( CHAR_DATA * ch, char *argument )
{
  OBJ_INDEX_DATA *pObjIndex;
  int hash, nMatch, type;
  char arg[MAX_INPUT_LENGTH];

  set_pager_color( AT_PLAIN, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Ofindtype what?\n\r", ch );
    return;
  }

/* If using this code outside of DBNA
 * Please do NOT remove this comment.
 *
 * telnet://dbnewage.com:3000         
 */

  nMatch = 0;
  type = get_otype( argument );
  if( type < 0 )
  {
    ch_printf( ch, "%s is an invalid item type.\n\r", argument );
    return;
  }

  for( hash = 0; hash < MAX_KEY_HASH; hash++ )
    for( pObjIndex = obj_index_hash[hash]; pObjIndex; pObjIndex = pObjIndex->next )
      if( type == pObjIndex->item_type )
      {
        nMatch++;
        pager_printf( ch, "[%5d] %s\n\r", pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
      }

  if( nMatch )
    pager_printf( ch, "Number of matches: %d\n", nMatch );
  else
    send_to_char( "Sorry, no matching item types found.\n\r", ch );
  return;
}

void do_mwhere( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  bool found;

  set_pager_color( AT_PLAIN, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Mwhere whom?\n\r", ch );
    return;
  }

  found = FALSE;
  for( victim = first_char; victim; victim = victim->next )
  {
    if( IS_NPC( victim ) && victim->in_room && nifty_is_name( arg, victim->name ) )
    {
      found = TRUE;
      pager_printf( ch, "[%5d] %-28s [%5d] %s\n\r",
                    victim->pIndexData->vnum, victim->short_descr, victim->in_room->vnum, victim->in_room->name );
    }
  }

  if( !found )
    act( AT_PLAIN, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
  return;
}

void do_gwhere( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA *d;
  bool found = FALSE, pmobs = FALSE;
  int low = 1, high = 65, count = 0;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if( arg1[0] != '\0' )
  {
    if( arg1[0] == '\0' || arg2[0] == '\0' )
    {
      send_to_pager_color( "\n\r&wSyntax:  gwhere | gwhere <low> <high> | gwhere <low> <high> mobs\n\r", ch );
      return;
    }
    low = atoi( arg1 );
    high = atoi( arg2 );
  }
  if( low < 1 || high < low || low > high || high > 65 )
  {
    send_to_pager_color( "&wInvalid level range.\n\r", ch );
    return;
  }
  argument = one_argument( argument, arg3 );
  if( !str_cmp( arg3, "mobs" ) )
    pmobs = TRUE;

  pager_printf_color( ch, "\n\r&cGlobal %s locations:&w\n\r", pmobs ? "mob" : "player" );
  if( !pmobs )
  {
    for( d = first_descriptor; d; d = d->next )
      if( ( d->connected == CON_PLAYING || d->connected == CON_EDITING )
          && ( victim = d->character ) != NULL && !IS_NPC( victim ) && victim->in_room
          && can_see( ch, victim ) && victim->level >= low && victim->level <= high )
      {
        found = TRUE;
        pager_printf_color( ch, "&c(&C%2d&c) &w%-12.12s   [%-5d - %-19.19s]   &c%-25.25s\n\r",
                            victim->level, victim->name, victim->in_room->vnum, victim->in_room->area->name,
                            victim->in_room->name );
        count++;
      }
  }
  else
  {
    for( victim = first_char; victim; victim = victim->next )
      if( IS_NPC( victim ) && victim->in_room && can_see( ch, victim ) && victim->level >= low && victim->level <= high )
      {
        found = TRUE;
        pager_printf_color( ch, "&c(&C%2d&c) &w%-12.12s   [%-5d - %-19.19s]   &c%-25.25s\n\r",
                            victim->level, victim->name, victim->in_room->vnum, victim->in_room->area->name,
                            victim->in_room->name );
        count++;
      }
  }
  pager_printf_color( ch, "&c%d %s found.\n\r", count, pmobs ? "mobs" : "characters" );
  return;
}

void do_gfighting( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  DESCRIPTOR_DATA *d;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  bool found = FALSE, pmobs = FALSE, phating = FALSE, phunting = FALSE;
  int low = 1, high = 65, count = 0;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if( arg1[0] != '\0' )
  {
    if( arg1[0] == '\0' || arg2[0] == '\0' )
    {
      send_to_pager_color( "\n\r&wSyntax:  gfighting | gfighting <low> <high> | gfighting <low> <high> mobs\n\r", ch );
      return;
    }
    low = atoi( arg1 );
    high = atoi( arg2 );
  }
  if( low < 1 || high < low || low > high || high > 65 )
  {
    send_to_pager_color( "&wInvalid level range.\n\r", ch );
    return;
  }
  argument = one_argument( argument, arg3 );
  if( !str_cmp( arg3, "mobs" ) )
    pmobs = TRUE;
  else if( !str_cmp( arg3, "hating" ) )
    phating = TRUE;
  else if( !str_cmp( arg3, "hunting" ) )
    phunting = TRUE;

  pager_printf_color( ch, "\n\r&cGlobal %s conflict:\n\r", pmobs ? "mob" : "character" );
  if( !pmobs && !phating && !phunting )
  {
    for( d = first_descriptor; d; d = d->next )
      if( ( d->connected == CON_PLAYING || d->connected == CON_EDITING )
          && ( victim = d->character ) != NULL && !IS_NPC( victim ) && victim->in_room
          && can_see( ch, victim ) && victim->fighting && victim->level >= low && victim->level <= high )
      {
        found = TRUE;
        pager_printf_color( ch, "&w%-12.12s &C|%2d &wvs &C%2d| &w%-16.16s [%5d]  &c%-20.20s [%5d]\n\r",
                            victim->name, victim->level, victim->fighting->who->level,
                            IS_NPC( victim->fighting->who ) ? victim->fighting->who->short_descr : victim->fighting->who->
                            name, IS_NPC( victim->fighting->who ) ? victim->fighting->who->pIndexData->vnum : 0,
                            victim->in_room->area->name, victim->in_room == NULL ? 0 : victim->in_room->vnum );
        count++;
      }
  }
  else if( !phating && !phunting )
  {
    for( victim = first_char; victim; victim = victim->next )
      if( IS_NPC( victim )
          && victim->in_room && can_see( ch, victim ) && victim->fighting && victim->level >= low && victim->level <= high )
      {
        found = TRUE;
        pager_printf_color( ch, "&w%-12.12s &C|%2d &wvs &C%2d| &w%-16.16s [%5d]  &c%-20.20s [%5d]\n\r",
                            victim->name, victim->level, victim->fighting->who->level,
                            IS_NPC( victim->fighting->who ) ? victim->fighting->who->short_descr : victim->fighting->who->
                            name, IS_NPC( victim->fighting->who ) ? victim->fighting->who->pIndexData->vnum : 0,
                            victim->in_room->area->name, victim->in_room == NULL ? 0 : victim->in_room->vnum );
        count++;
      }
  }
  else if( !phunting && phating )
  {
    for( victim = first_char; victim; victim = victim->next )
      if( IS_NPC( victim )
          && victim->in_room && can_see( ch, victim ) && victim->hating && victim->level >= low && victim->level <= high )
      {
        found = TRUE;
        pager_printf_color( ch, "&w%-12.12s &C|%2d &wvs &C%2d| &w%-16.16s [%5d]  &c%-20.20s [%5d]\n\r",
                            victim->name, victim->level, victim->hating->who->level, IS_NPC( victim->hating->who ) ?
                            victim->hating->who->short_descr : victim->hating->who->name, IS_NPC( victim->hating->who ) ?
                            victim->hating->who->pIndexData->vnum : 0, victim->in_room->area->name,
                            victim->in_room == NULL ? 0 : victim->in_room->vnum );
        count++;
      }
  }
  else if( phunting )
  {
    for( victim = first_char; victim; victim = victim->next )
      if( IS_NPC( victim )
          && victim->in_room && can_see( ch, victim ) && victim->hunting && victim->level >= low && victim->level <= high )
      {
        found = TRUE;
        pager_printf_color( ch, "&w%-12.12s &C|%2d &wvs &C%2d| &w%-16.16s [%5d]  &c%-20.20s [%5d]\n\r",
                            victim->name, victim->level, victim->hunting->who->level, IS_NPC( victim->hunting->who ) ?
                            victim->hunting->who->short_descr : victim->hunting->who->name,
                            IS_NPC( victim->hunting->who ) ? victim->hunting->who->pIndexData->vnum : 0,
                            victim->in_room->area->name, victim->in_room == NULL ? 0 : victim->in_room->vnum );
        count++;
      }
  }
  pager_printf_color( ch, "&c%d %s conflicts located.\n\r", count, pmobs ? "mob" : "character" );
  return;
}

/* Added 'show' argument for lowbie imms without ostat -- Blodkai */
/* Made show the default action :) Shaddai */
/* Trimmed size, added vict info, put lipstick on the pig -- Blod */
void do_bodybag( CHAR_DATA * ch, char *argument )
{
  char buf2[MAX_STRING_LENGTH];
  char buf3[MAX_STRING_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *owner;
  OBJ_DATA *obj;
  bool found = FALSE, bag = FALSE;

  argument = one_argument( argument, arg1 );
  if( arg1[0] == '\0' )
  {
    send_to_char_color( "&PSyntax:  bodybag <character> | bodybag <character> yes/bag/now\n\r", ch );
    return;
  }

  sprintf( buf3, " " );
  sprintf( buf2, "the corpse of %s", arg1 );
  argument = one_argument( argument, arg2 );

  if( arg2[0] != '\0' && ( str_cmp( arg2, "yes" ) && str_cmp( arg2, "bag" ) && str_cmp( arg2, "now" ) ) )
  {
    send_to_char_color( "\n\r&PSyntax:  bodybag <character> | bodybag <character> yes/bag/now\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "yes" ) || !str_cmp( arg2, "bag" ) || !str_cmp( arg2, "now" ) )
    bag = TRUE;

  pager_printf_color( ch, "\n\r&P%s remains of %s ... ", bag ? "Retrieving" : "Searching for", capitalize( arg1 ) );
  for( obj = first_object; obj; obj = obj->next )
  {
    if( obj->in_room && !str_cmp( buf2, obj->short_descr ) && ( obj->pIndexData->vnum == 11 ) )
    {
      send_to_pager( "\n\r", ch );
      found = TRUE;
      pager_printf_color( ch, "&P%s:  %s%-12.12s %s  &PIn:  &w%-22.22s  &P[&w%5d&P]   &PTimer:  %s%2d",
                          bag ? "Bagging" : "Corpse",
                          bag ? "&Y" : "&w",
                          capitalize( arg1 ),
                          IS_OBJ_STAT( obj, ITEM_CLANCORPSE ) ? "&RPK" : "&R  ",
                          obj->in_room->area->name,
                          obj->in_room->vnum,
                          obj->timer < 1 ? "&w" : obj->timer < 5 ? "&R" : obj->timer < 10 ? "&Y" : "&w", obj->timer );
      if( bag )
      {
        obj_from_room( obj );
        obj = obj_to_char( obj, ch );
        obj->timer = -1;
        save_char_obj( ch );
      }
    }
  }
  if( !found )
  {
    send_to_pager_color( "&Pno corpse was found.\n\r", ch );
    return;
  }
  send_to_pager( "\n\r", ch );
  for( owner = first_char; owner; owner = owner->next )
  {
    if( IS_NPC( owner ) )
      continue;
    if( can_see( ch, owner ) && !str_cmp( arg1, owner->name ) )
      break;
  }
  if( owner == NULL )
  {
    pager_printf_color( ch, "&P%s is not currently online.\n\r", capitalize( arg1 ) );
    return;
  }
  if( owner->pcdata->deity )
    pager_printf_color( ch, "&P%s (%d) has %d favor with %s (needed to supplicate: %d)\n\r",
                        owner->name,
                        owner->level, owner->pcdata->favor, owner->pcdata->deity->name, owner->pcdata->deity->scorpse );
  else
    pager_printf_color( ch, "&P%s (%d) has no deity.\n\r", owner->name, owner->level );
  return;
}

#if 0
/* New owhere by Altrag, 03/14/96 */
void do_owhere( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  bool found;
  int icnt = 0;

  set_pager_color( AT_PLAIN, ch );

  argument = one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Owhere what?\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg1 );
  if( arg1[0] != '\0' && !str_prefix( arg1, "nesthunt" ) )
  {
    if( !( obj = get_obj_world( ch, arg ) ) )
    {
      send_to_char( "Nesthunt for what object?\n\r", ch );
      return;
    }
    for( ; obj->in_obj; obj = obj->in_obj )
    {
      pager_printf( ch, "[%5d] %-28s in object [%5d] %s\n\r",
                    obj->pIndexData->vnum, obj_short( obj ), obj->in_obj->pIndexData->vnum, obj->in_obj->short_descr );
      ++icnt;
    }
    sprintf( buf, "[%5d] %-28s in ", obj->pIndexData->vnum, obj_short( obj ) );
    if( obj->carried_by )
      sprintf( buf + strlen( buf ), "invent [%5d] %s\n\r",
               ( IS_NPC( obj->carried_by ) ? obj->carried_by->pIndexData->vnum : 0 ), PERS( obj->carried_by, ch ) );
    else if( obj->in_room )
      sprintf( buf + strlen( buf ), "room   [%5d] %s\n\r", obj->in_room->vnum, obj->in_room->name );
    else if( obj->in_obj )
    {
      bug( "do_owhere: obj->in_obj after NULL!", 0 );
      strcat( buf, "object??\n\r" );
    }
    else
    {
      bug( "do_owhere: object doesnt have location!", 0 );
      strcat( buf, "nowhere??\n\r" );
    }
    send_to_pager( buf, ch );
    ++icnt;
    pager_printf( ch, "Nested %d levels deep.\n\r", icnt );
    return;
  }

  found = FALSE;
  for( obj = first_object; obj; obj = obj->next )
  {
    if( !nifty_is_name( arg, obj->name ) )
      continue;
    found = TRUE;

    sprintf( buf, "(%3d) [%5d] %-28s in ", ++icnt, obj->pIndexData->vnum, obj_short( obj ) );
    if( obj->carried_by )
      sprintf( buf + strlen( buf ), "invent [%5d] %s\n\r",
               ( IS_NPC( obj->carried_by ) ? obj->carried_by->pIndexData->vnum : 0 ), PERS( obj->carried_by, ch ) );
    else if( obj->in_room )
      sprintf( buf + strlen( buf ), "room   [%5d] %s\n\r", obj->in_room->vnum, obj->in_room->name );
    else if( obj->in_obj )
      sprintf( buf + strlen( buf ), "object [%5d] %s\n\r", obj->in_obj->pIndexData->vnum, obj_short( obj->in_obj ) );
    else
    {
      bug( "do_owhere: object doesnt have location!", 0 );
      strcat( buf, "nowhere??\n\r" );
    }
    send_to_pager( buf, ch );
  }

  if( !found )
    act( AT_PLAIN, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
  else
    pager_printf( ch, "%d matches.\n\r", icnt );
  return;
}
#endif

void do_reboo( CHAR_DATA * ch, char *argument )
{
  send_to_char_color( "&YIf you want to REBOOT, spell it out.\n\r", ch );
  return;
}

void do_reboot( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  extern bool mud_down;
  CHAR_DATA *vch;

  set_char_color( AT_IMMORT, ch );

  if( str_cmp( argument, "mud now" ) && str_cmp( argument, "nosave" ) && str_cmp( argument, "and sort skill table" ) )
  {
    send_to_char( "Syntax:  'reboot mud now' or 'reboot nosave'\n\r", ch );
    return;
  }

  if( auction->item )
    do_auction( ch, "stop" );
  sprintf( buf, "Reboot by %s.", ch->name );
  do_echo( ch, buf );

  if( !str_cmp( argument, "and sort skill table" ) )
  {
    sort_skill_table(  );
    save_skill_table(  );
  }

  /*
   * Save all characters before booting. 
   */
  if( str_cmp( argument, "nosave" ) )
    for( vch = first_char; vch; vch = vch->next )
      if( !IS_NPC( vch ) )
        save_char_obj( vch );

  mud_down = TRUE;
  return;
}

void do_shutdow( CHAR_DATA * ch, char *argument )
{
  send_to_char_color( "&YIf you want to SHUTDOWN, spell it out.\n\r", ch );
  return;
}

void do_shutdown( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  extern bool mud_down;
  CHAR_DATA *vch;

  set_char_color( AT_IMMORT, ch );

  if( str_cmp( argument, "mud now" ) && str_cmp( argument, "nosave" ) )
  {
    send_to_char( "Syntax:  'shutdown mud now' or 'shutdown nosave'\n\r", ch );
    return;
  }

  if( auction->item )
    do_auction( ch, "stop" );
  sprintf( buf, "Shutdown by %s.", ch->name );
  append_file( ch, SHUTDOWN_FILE, buf );
  strcat( buf, "\n\r" );
  do_echo( ch, buf );

  /*
   * Save all characters before booting. 
   */
  if( str_cmp( argument, "this nosave" ) )
    for( vch = first_char; vch; vch = vch->next )
      if( !IS_NPC( vch ) )
        save_char_obj( vch );
  mud_down = TRUE;
  return;
}

void do_snoop( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Snoop whom?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( !victim->desc )
  {
    send_to_char( "No descriptor to snoop.\n\r", ch );
    return;
  }
  if( victim == ch )
  {
    send_to_char( "Cancelling all snoops.\n\r", ch );
    for( d = first_descriptor; d; d = d->next )
      if( d->snoop_by == ch->desc )
      {
        d->snoop_by = NULL;
      }
    return;
  }
  if( victim->desc->snoop_by )
  {
    send_to_char( "Busy already.\n\r", ch );
    return;
  }

  /*
   * Minimum snoop level... a secret mset value
   * makes the snooper think that the victim is already being snooped
   */
  if( get_trust( victim ) >= get_trust( ch ) || ( victim->pcdata && victim->pcdata->min_snoop > get_trust( ch ) ) )
  {
    send_to_char( "Busy already.\n\r", ch );
    return;
  }

  if( ch->desc )
  {
    for( d = ch->desc->snoop_by; d; d = d->snoop_by )
      if( d->character == victim || d->original == victim )
      {
        send_to_char( "No snoop loops.\n\r", ch );
        return;
      }
  }

  /*
   * Snoop notification for higher imms, if desired, uncomment this 
   */
#ifdef TOOSNOOPY
  if( get_trust( victim ) > LEVEL_GOD && get_trust( ch ) < LEVEL_SUPREME )
    write_to_descriptor( victim->desc->descriptor, "\n\rYou feel like someone is watching your every move...\n\r", 0 );
#endif
  victim->desc->snoop_by = ch->desc;
  send_to_char( "Ok.\n\r", ch );
  return;
}

void do_statshield( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( IS_NPC( ch ) || get_trust( ch ) < LEVEL_GREATER )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( arg[0] == '\0' )
  {
    send_to_char( "Statshield which mobile?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "No such mobile.\n\r", ch );
    return;
  }
  if( !IS_NPC( victim ) )
  {
    send_to_char( "You can only statshield mobiles.\n\r", ch );
    return;
  }
  if( xIS_SET( victim->act, ACT_STATSHIELD ) )
  {
    xREMOVE_BIT( victim->act, ACT_STATSHIELD );
    ch_printf( ch, "You have lifted the statshield on %s.\n\r", victim->short_descr );
  }
  else
  {
    xSET_BIT( victim->act, ACT_STATSHIELD );
    ch_printf( ch, "You have applied a statshield to %s.\n\r", victim->short_descr );
  }
  return;
}


void do_switch( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Switch into whom?\n\r", ch );
    return;
  }
  if( !ch->desc )
    return;
  if( ch->desc->original )
  {
    send_to_char( "You are already switched.\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( victim == ch )
  {
    send_to_char( "Ok.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) && xIS_SET( victim->act, ACT_STATSHIELD ) && get_trust( ch ) < LEVEL_GREATER )
  {
    set_pager_color( AT_IMMORT, ch );
    send_to_pager( "Their godly glow prevents you from getting close enough.\n\r", ch );
    return;
  }
  if( victim->desc )
  {
    send_to_char( "Character in use.\n\r", ch );
    return;
  }
  if( !IS_NPC( victim ) && ch->level < LEVEL_GREATER )
  {
    send_to_char( "You cannot switch into a player!\n\r", ch );
    return;
  }
  if( victim->switched )
  {
    send_to_char( "You can't switch into a player that is switched!\n\r", ch );
    return;
  }
  if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_FREEZE ) )
  {
    send_to_char( "You shouldn't switch into a player that is frozen!\n\r", ch );
    return;
  }
  ch->desc->character = victim;
  ch->desc->original = ch;
  victim->desc = ch->desc;
  ch->desc = NULL;
  ch->switched = victim;
  send_to_char( "Ok.\n\r", victim );
  return;
}

void do_return( CHAR_DATA * ch, char *argument )
{

  if( !IS_NPC( ch ) && get_trust( ch ) < LEVEL_IMMORTAL )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  set_char_color( AT_IMMORT, ch );

  if( !ch->desc )
    return;
  if( !ch->desc->original )
  {
    send_to_char( "You aren't switched.\n\r", ch );
    return;
  }

  send_to_char( "You return to your original body.\n\r", ch );

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_POSSESS ) )
  {
    affect_strip( ch, gsn_possess );
    xREMOVE_BIT( ch->affected_by, AFF_POSSESS );
  }

  ch->desc->character = ch->desc->original;
  ch->desc->original = NULL;
  ch->desc->character->desc = ch->desc;
  ch->desc->character->switched = NULL;
  ch->desc = NULL;
  return;
}

void do_minvoke( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  MOB_INDEX_DATA *pMobIndex;
  CHAR_DATA *victim;
  int vnum;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Syntax:  minvoke <vnum>\n\r", ch );
    return;
  }
  if( !is_number( arg ) )
  {
    char arg2[MAX_INPUT_LENGTH];
    int hash, cnt;
    int count = number_argument( arg, arg2 );

    vnum = -1;
    for( hash = cnt = 0; hash < MAX_KEY_HASH; hash++ )
      for( pMobIndex = mob_index_hash[hash]; pMobIndex; pMobIndex = pMobIndex->next )
        if( nifty_is_name( arg2, pMobIndex->player_name ) && ++cnt == count )
        {
          vnum = pMobIndex->vnum;
          break;
        }
    if( vnum == -1 )
    {
      send_to_char( "No such mobile exists.\n\r", ch );
      return;
    }
  }
  else
    vnum = atoi( arg );

  if( get_trust( ch ) < sysdata.level_modify_proto )
  {
    AREA_DATA *pArea;

    if( IS_NPC( ch ) )
    {
      send_to_char( "Huh?\n\r", ch );
      return;
    }
    if( !ch->pcdata || !( pArea = ch->pcdata->area ) )
    {
      send_to_char( "You must have an assigned area to invoke this mobile.\n\r", ch );
      return;
    }
    if( vnum < pArea->low_m_vnum && vnum > pArea->hi_m_vnum )
    {
      send_to_char( "That number is not in your allocated range.\n\r", ch );
      return;
    }
  }
  if( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
  {
    send_to_char( "No mobile has that vnum.\n\r", ch );
    return;
  }

  if( !xIS_SET( pMobIndex->act, ACT_PROTOTYPE ) && ch->level < sysdata.level_modify_proto )
  {
    send_to_char( "You can't invoke non-prototype mobs.\n\r", ch );
    return;
  }

  victim = create_mobile( pMobIndex );
  char_to_room( victim, ch->in_room );
  act( AT_IMMORT, "$n invokes $N!", ch, NULL, victim, TO_ROOM );
  /*
   * How about seeing what we're invoking for a change. -Blodkai
   */
  ch_printf_color( ch, "&YYou invoke %s (&W#%d &Y- &W%s &Y- &Wlvl %d&Y)\n\r",
                   pMobIndex->short_descr, pMobIndex->vnum, pMobIndex->player_name, victim->level );
  return;
}

void do_oinvoke( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  OBJ_INDEX_DATA *pObjIndex;
  OBJ_DATA *obj;
  int vnum;
  int level;


  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if( arg1[0] == '\0' )
  {
    send_to_char( "Syntax: oinvoke <vnum> <level>.\n\r", ch );
    return;
  }
  if( arg2[0] == '\0' )
  {
    level = get_trust( ch );
  }
  else
  {
    if( !is_number( arg2 ) )
    {
      send_to_char( "Syntax:  oinvoke <vnum> <level>\n\r", ch );
      return;
    }
    level = atoi( arg2 );
    if( level < 0 || level > get_trust( ch ) )
    {
      send_to_char( "Limited to your trust level.\n\r", ch );
      return;
    }
  }
  if( !is_number( arg1 ) )
  {
    char arg[MAX_INPUT_LENGTH];
    int hash, cnt;
    int count = number_argument( arg1, arg );

    vnum = -1;
    for( hash = cnt = 0; hash < MAX_KEY_HASH; hash++ )
      for( pObjIndex = obj_index_hash[hash]; pObjIndex; pObjIndex = pObjIndex->next )
        if( nifty_is_name( arg, pObjIndex->name ) && ++cnt == count )
        {
          vnum = pObjIndex->vnum;
          break;
        }
    if( vnum == -1 )
    {
      send_to_char( "No such object exists.\n\r", ch );
      return;
    }
  }
  else
    vnum = atoi( arg1 );

  if( ch->level < sysdata.level_modify_proto && ch->level != 53 )
  {
    AREA_DATA *pArea;

    if( IS_NPC( ch ) )
    {
      send_to_char( "Huh?\n\r", ch );
      return;
    }
    if( !ch->pcdata || !( pArea = ch->pcdata->area ) )
    {
      send_to_char( "You must have an assigned area to invoke this object.\n\r", ch );
      return;
    }
    if( vnum < pArea->low_o_vnum && vnum > pArea->hi_o_vnum )
    {
      send_to_char( "That number is not in your allocated range.\n\r", ch );
      return;
    }
  }
  if( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
  {
    send_to_char( "No object has that vnum.\n\r", ch );
    return;
  }

  if( !IS_OBJ_STAT( pObjIndex, ITEM_PROTOTYPE ) && get_trust( ch ) < sysdata.level_modify_proto && ch->level != 53 )
  {
    send_to_char( "You can't invoke non-prototype objects.\n\r", ch );
    return;
  }

  if( ( pObjIndex->item_type == ITEM_DRAGONBALL || pObjIndex->item_type == ITEM_DRAGONRADAR )  && ch->level < 63 )
  {
    ch_printf( ch, "Access denied.\n\r" );
    return;
  }

  if( level == 0 )
  {
    AREA_DATA *temp_area;

    if( ( temp_area = get_area_obj( pObjIndex ) ) == NULL )
      level = ch->level;
    else
    {
      level = generate_itemlevel( temp_area, pObjIndex );
      level = URANGE( 0, level, LEVEL_AVATAR );
    }
  }


  obj = create_object_new( pObjIndex, level, ORIGIN_OINVOKE, ch->name );
  if( CAN_WEAR( obj, ITEM_TAKE ) )
  {
    obj = obj_to_char( obj, ch );
  }
  else
  {
    obj = obj_to_room( obj, ch->in_room );
    act( AT_IMMORT, "$n fashions $p from ether!", ch, obj, NULL, TO_ROOM );
  }
  /*
   * I invoked what? --Blodkai 
   */
  ch_printf_color( ch, "&YYou invoke %s (&W#%d &Y- &W%s &Y- &Wlvl %d&Y)\n\r",
                   pObjIndex->short_descr, pObjIndex->vnum, pObjIndex->name, obj->level );
  return;
}

void do_purge( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    /*
     * 'purge' 
     */
    CHAR_DATA *vnext;
    OBJ_DATA *obj_next;

    for( victim = ch->in_room->first_person; victim; victim = vnext )
    {
      vnext = victim->next_in_room;
      if( IS_NPC( victim ) && victim != ch )
        extract_char( victim, TRUE );
    }

    for( obj = ch->in_room->first_content; obj; obj = obj_next )
    {
      obj_next = obj->next_content;
      extract_obj( obj );
    }

    act( AT_IMMORT, "$n purges the room!", ch, NULL, NULL, TO_ROOM );
    act( AT_IMMORT, "You have purged the room!", ch, NULL, NULL, TO_CHAR );
    save_house_by_vnum( ch->in_room->vnum );  /* Prevent House Object Duplication */
    return;
  }
  victim = NULL;
  obj = NULL;

  /*
   * fixed to get things in room first -- i.e., purge portal (obj),
   * * no more purging mobs with that keyword in another room first
   * * -- Tri 
   */
  if( ( victim = get_char_room( ch, arg ) ) == NULL && ( obj = get_obj_here( ch, arg ) ) == NULL )
  {
    if( ( victim = get_char_world( ch, arg ) ) == NULL && ( obj = get_obj_world( ch, arg ) ) == NULL )  /* no get_obj_room */
    {
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }
  }

  /*
   * Single object purge in room for high level purge - Scryn 8/12
   */
  if( obj )
  {
    int vnum = ( obj->in_room ? obj->in_room->vnum : ROOM_VNUM_LIMBO );
    separate_obj( obj );
    act( AT_IMMORT, "$n purges $p.", ch, obj, NULL, TO_ROOM );
    act( AT_IMMORT, "You make $p disappear in a puff of smoke!", ch, obj, NULL, TO_CHAR );
    extract_obj( obj );
    save_house_by_vnum( vnum ); /* Prevent House Object Duplication */
    return;
  }

  if( !IS_NPC( victim ) )
  {
    send_to_char( "Not on PC's.\n\r", ch );
    return;
  }

  if( victim == ch )
  {
    send_to_char( "You cannot purge yourself!\n\r", ch );
    return;
  }

  act( AT_IMMORT, "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
  act( AT_IMMORT, "You make $N disappear in a puff of smoke!", ch, NULL, victim, TO_CHAR );
  extract_char( victim, TRUE );
  return;
}

void do_low_purge( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Purge what?\n\r", ch );
    return;
  }

  victim = NULL;
  obj = NULL;
  if( ( victim = get_char_room( ch, arg ) ) == NULL && ( obj = get_obj_here( ch, arg ) ) == NULL )
  {
    send_to_char( "You can't find that here.\n\r", ch );
    return;
  }

  if( obj )
  {
    separate_obj( obj );
    act( AT_IMMORT, "$n purges $p!", ch, obj, NULL, TO_ROOM );
    act( AT_IMMORT, "You make $p disappear in a puff of smoke!", ch, obj, NULL, TO_CHAR );
    extract_obj( obj );
    return;
  }

  if( !IS_NPC( victim ) )
  {
    send_to_char( "Not on PC's.\n\r", ch );
    return;
  }

  if( victim == ch )
  {
    send_to_char( "You cannot purge yourself!\n\r", ch );
    return;
  }

  act( AT_IMMORT, "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
  act( AT_IMMORT, "You make $N disappear in a puff of smoke!", ch, NULL, victim, TO_CHAR );
  extract_char( victim, TRUE );
  return;
}

void do_balzhur( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char *name;
  CHAR_DATA *victim;
  AREA_DATA *pArea;
  int sn;

  set_char_color( AT_BLOOD, ch );

  argument = one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Who is deserving of such a fate?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't currently playing.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "This will do little good on mobiles.\n\r", ch );
    return;
  }
  if( victim->level >= get_trust( ch ) )
  {
    send_to_char( "I wouldn't even think of that if I were you...\n\r", ch );
    return;
  }

  set_char_color( AT_WHITE, ch );
  send_to_char( "You summon the demon Balzhur to wreak your wrath!\n\r", ch );
  send_to_char( "Balzhur sneers at you evilly, then vanishes in a puff of smoke.\n\r", ch );
  set_char_color( AT_IMMORT, victim );
  send_to_char( "You hear an ungodly sound in the distance that makes your blood run cold!\n\r", victim );
  sprintf( buf, "&YBalzhur screams, 'You are MINE %s!!!'&D", victim->name );
  echo_to_all( AT_IMMORT, buf, ECHOTAR_ALL );
  victim->level = 1;
//  victim->trust = 0;
  victim->exp = 100;
  victim->max_hit = 100;
  victim->max_mana = 100;
  victim->max_move = 100;
  for( sn = 0; sn < top_sn; sn++ )
    victim->pcdata->learned[sn] = 0;
  victim->practice = 0;
  victim->train = 0;
  victim->hit = victim->max_hit;
  victim->mana = victim->max_mana;
  victim->train = victim->max_move;
  name = capitalize( victim->name );
  sprintf( buf, "%s%s", GOD_DIR, name );

  set_char_color( AT_RED, ch );
  if( !remove( buf ) )
    send_to_char( "Player's immortal data destroyed.\n\r", ch );
  else if( errno != ENOENT )
  {
    ch_printf( ch, "Unknown error #%d - %s (immortal data).  Report to Coder.\n\r", errno, strerror( errno ) );
    sprintf( buf2, "%s balzhuring %s", ch->name, buf );
    perror( buf2 );
  }
  sprintf( buf2, "%s.are", name );
  for( pArea = first_build; pArea; pArea = pArea->next )
    if( !str_cmp( pArea->filename, buf2 ) )
    {
      sprintf( buf, "%s%s", BUILD_DIR, buf2 );
      if( IS_SET( pArea->status, AREA_LOADED ) )
        fold_area( pArea, buf, FALSE );
      close_area( pArea );
      sprintf( buf2, "%s.bak", buf );
      set_char_color( AT_RED, ch ); /* Log message changes colors */
      if( !rename( buf, buf2 ) )
        send_to_char( "Player's area data destroyed.  Area saved as backup.\n\r", ch );
      else if( errno != ENOENT )
      {
        ch_printf( ch, "Unknown error #%d - %s (area data).  Report to  Thoric.\n\r", errno, strerror( errno ) );
        sprintf( buf2, "%s destroying %s", ch->name, buf );
        perror( buf2 );
      }
      break;
    }

  make_wizlist(  );
  advance_level( victim );
  do_help( victim, "M_BALZHUR_" );
  set_char_color( AT_WHITE, victim );
  send_to_char( "You awake after a long period of time...\n\r", victim );
  while( victim->first_carrying )
    extract_obj( victim->first_carrying );
  return;
}

void do_advance( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int level;
  int iLevel;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
  {
    send_to_char( "Syntax:  advance <character> <level>\n\r", ch );
    return;
  }
  if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    send_to_char( "That character is not in the room.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "You cannot advance a mobile.\n\r", ch );
    return;
  }
  /*
   * You can demote yourself but not someone else at your own trust.-- Narn
   */
  if( get_trust( ch ) <= get_trust( victim ) && ch != victim )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }
  if( ( level = atoi( arg2 ) ) < 1 || level > MAX_LEVEL )
  {
    send_to_char( "Level range is 1 to 65.\n\r", ch );
    return;
  }
  if( level > get_trust( ch ) )
  {
    send_to_char( "Level limited to your trust level.\n\r", ch );
    return;
  }
  /*
   * Lower level:
   * *   Reset to level 1.
   * *   Then raise again.
   * *   Currently, an imp can lower another imp.
   * *   -- Swiftest
   * *   Can't lower imms >= your trust (other than self) per Narn's change.
   * *   Few minor text changes as well.  -- Blod
   */
  if( level <= victim->level )
  {
    int sn;

    set_char_color( AT_IMMORT, victim );
    /*
     * Fixed bug here, victim->level has not been changed here, thus their level is
     * STILL going to be above LEVEL_AVATAR, changed to just 'level' as that's what
     * they're being advanced to. - Orion Elder 
     */
    if( level <= LEVEL_AVATAR && IS_IMMORTAL( victim ) )
    {
      if( victim->pcdata->bestowments )
        DISPOSE( victim->pcdata->bestowments );
      victim->pcdata->bestowments = str_dup( "" );
      xREMOVE_BIT( victim->act, PLR_HOLYLIGHT );
      if( !IS_RETIRED( victim ) )
      {
        /*
         * Fixed bug here, was removing the immortal data of the person
         * who used advance - Orion Elder
         */
        sprintf( buf, "%s%s", GOD_DIR, capitalize( victim->name ) );

        /*
         * Added to notify of removal of Immortal data. - Orion Elder 
         */
        if( !remove( buf ) )
          send_to_char( "Player's immortal data destroyed.\n\r", ch );
      }
    }
    if( level < victim->level )
    {
      ch_printf( ch, "Demoting %s from level %d to level %d!\n\r", victim->name, victim->level, level );
      send_to_char( "Cursed and forsaken!  The gods have lowered your level...\n\r", victim );
    }
    else
    {
      ch_printf( ch, "%s is already level %d.  Re-advancing...\n\r", victim->name, level );
      send_to_char( "Deja vu!  Your mind reels as you re-live your past levels!\n\r", victim );
    }
    victim->level = 1;
    //        victim->exp      = exp_level(victim, 1);
    victim->max_hit = 100;
    victim->max_mana = 100;
    victim->max_move = 100;
    for( sn = 0; sn < top_sn; sn++ )
      victim->pcdata->learned[sn] = 0;
    victim->practice = 0;
    victim->train = 0;
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    advance_level( victim );
    /*
     * Rank fix added by Narn. 
     */
    if( victim->pcdata->pretitle )
      STRFREE( victim->pcdata->pretitle );
    victim->pcdata->pretitle = str_dup( "" );
    /*
     * Stuff added to make sure character's wizinvis level doesn't stay
     * higher than actual level, take wizinvis away from advance < 50 
     */
    if( xIS_SET( victim->act, PLR_WIZINVIS ) )
      victim->pcdata->wizinvis = get_trust( victim );
    if( xIS_SET( victim->act, PLR_WIZINVIS ) && ( victim->level <= LEVEL_AVATAR ) )
    {
      xREMOVE_BIT( victim->act, PLR_WIZINVIS );
      victim->pcdata->wizinvis = get_trust( victim );
    }
  }
  else
  {
    ch_printf( ch, "Raising %s from level %d to level %d!\n\r", victim->name, victim->level, level );
    if( victim->level >= LEVEL_AVATAR )
    {
      set_char_color( AT_IMMORT, victim );
      act( AT_IMMORT, "$n makes some arcane gestures with $s hands, then points $s finger at you!",
           ch, NULL, victim, TO_VICT );
      act( AT_IMMORT, "$n makes some arcane gestures with $s hands, then points $s finger at $N!",
           ch, NULL, victim, TO_NOTVICT );
      set_char_color( AT_WHITE, victim );
      send_to_char( "You suddenly feel very strange...\n\r\n\r", victim );
      set_char_color( AT_LBLUE, victim );
    }
    switch ( level )
    {
      default:
        send_to_char( "The gods feel fit to raise your level!\n\r", victim );
        break;
      case LEVEL_IMMORTAL:
        do_help( victim, "M_GODLVL1_" );
        set_char_color( AT_WHITE, victim );
        send_to_char( "You awake... all your possessions are gone.\n\r", victim );
        while( victim->first_carrying )
          extract_obj( victim->first_carrying );
        break;
      case LEVEL_ACOLYTE:
        do_help( victim, "M_GODLVL2_" );
        break;
      case LEVEL_CREATOR:
        do_help( victim, "M_GODLVL3_" );
        break;
      case LEVEL_SAVIOR:
        do_help( victim, "M_GODLVL4_" );
        break;
      case LEVEL_DEMI:
        do_help( victim, "M_GODLVL5_" );
        break;
      case LEVEL_TRUEIMM:
        do_help( victim, "M_GODLVL6_" );
        break;
      case LEVEL_LESSER:
        do_help( victim, "M_GODLVL7_" );
        break;
      case LEVEL_GOD:
        do_help( victim, "M_GODLVL8_" );
        break;
      case LEVEL_GREATER:
        do_help( victim, "M_GODLVL9_" );
        break;
      case LEVEL_ASCENDANT:
        do_help( victim, "M_GODLVL10_" );
        break;
      case LEVEL_SUB_IMPLEM:
        do_help( victim, "M_GODLVL11_" );
        break;
      case LEVEL_IMPLEMENTOR:
        do_help( victim, "M_GODLVL12_" );
        break;
      case LEVEL_ETERNAL:
        do_help( victim, "M_GODLVL13_" );
        break;
      case LEVEL_INFINITE:
        do_help( victim, "M_GODLVL14_" );
        break;
      case LEVEL_SUPREME:
        do_help( victim, "M_GODLVL15_" );
    }
  }
  for( iLevel = victim->level; iLevel < level; iLevel++ )
  {
    if( level < LEVEL_IMMORTAL )
      send_to_char( "You raise a level!!\n\r", victim );
    victim->level += 1;
    advance_level( victim );
  }
  //    victim->exp   = exp_level( victim, victim->level );
//                victim->trust = 0;
  return;
}

void do_elevate( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Syntax: elevate <char>\n\r", ch );
    return;
  }
  if( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char( "That player is not here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }
  if( victim->level == LEVEL_IMMORTAL )
  {
    send_to_char( "Elevating a player...\n\r", ch );
    set_char_color( AT_IMMORT, victim );
    act( AT_IMMORT, "$n begins to chant softly... then makes some arcane gestures...", ch, NULL, NULL, TO_ROOM );
    set_char_color( AT_WHITE, victim );
    send_to_char( "You suddenly feel very strange...\n\r\n\r", victim );
    set_char_color( AT_LBLUE, victim );
    do_help( victim, "M_GODLVL2_" );
    victim->level = LEVEL_ACOLYTE;
    set_char_color( AT_WHITE, victim );
    advance_level( victim );
    //       victim->exp   = exp_level( victim, victim->level );
//                  victim->trust = 0;
    return;
  }
  if( victim->level == LEVEL_ACOLYTE )
  {
    send_to_char( "Elevating a player...\n\r", ch );
    set_char_color( AT_IMMORT, victim );
    act( AT_IMMORT, "$n begins to chant softly... then makes some arcane gestures...", ch, NULL, NULL, TO_ROOM );
    set_char_color( AT_WHITE, victim );
    send_to_char( "You suddenly feel very strange...\n\r\n\r", victim );
    set_char_color( AT_LBLUE, victim );
    do_help( victim, "M_GODLVL3_" );
    victim->level = LEVEL_CREATOR;
    set_char_color( AT_WHITE, victim );
    advance_level( victim );
    //       victim->exp   = exp_level( victim, victim->level );
//                  victim->trust = 0;
    return;
  }
  else
    send_to_char( "You cannot elevate this character.\n\r", ch );
  return;
}

              /*
               * Summer 1997 --Blod 
               */
void do_scatter( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *pRoomIndex;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Scatter whom?\n\r", ch );
    return;
  }
  if( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( victim == ch )
  {
    send_to_char( "It's called teleport.  Try it.\n\r", ch );
    return;
  }
  if( !IS_NPC( victim ) && get_trust( victim ) >= get_trust( ch ) )
  {
    send_to_char( "You haven't the power to succeed against them.\n\r", ch );
    return;
  }
  for( ;; )
  {
    pRoomIndex = get_room_index( number_range( 0, MAX_VNUMS ) );
    if( pRoomIndex )
      if( !xIS_SET( pRoomIndex->room_flags, ROOM_PRIVATE )
          && !xIS_SET( pRoomIndex->room_flags, ROOM_SOLITARY )
          && !xIS_SET( pRoomIndex->room_flags, ROOM_NO_ASTRAL ) && !xIS_SET( pRoomIndex->room_flags, ROOM_PROTOTYPE ) )
        break;
  }
  if( victim->fighting )
    stop_fighting( victim, TRUE );
  act( AT_MAGIC, "With the sweep of an arm, $n flings $N to the winds.", ch, NULL, victim, TO_NOTVICT );
  act( AT_MAGIC, "With the sweep of an arm, $n flings you to the astral winds.", ch, NULL, victim, TO_VICT );
  act( AT_MAGIC, "With the sweep of an arm, you fling $N to the astral winds.", ch, NULL, victim, TO_CHAR );
  char_from_room( victim );
  char_to_room( victim, pRoomIndex );
  victim->position = POS_RESTING;
  act( AT_MAGIC, "$n staggers forth from a sudden gust of wind, and collapses.", victim, NULL, NULL, TO_ROOM );
  do_look( victim, "auto" );
  return;
}

void do_strew( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj_next;
  OBJ_DATA *obj_lose;
  ROOM_INDEX_DATA *pRoomIndex;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if( arg1[0] == '\0' || arg2[0] == '\0' )
  {
    send_to_char( "Strew who, what?\n\r", ch );
    return;
  }
  if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    send_to_char( "It would work better if they were here.\n\r", ch );
    return;
  }
  if( victim == ch )
  {
    send_to_char( "Try taking it out on someone else first.\n\r", ch );
    return;
  }
  if( !IS_NPC( victim ) && get_trust( victim ) >= get_trust( ch ) )
  {
    send_to_char( "You haven't the power to succeed against them.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "zeni" ) )
  {
    if( victim->gold < 1 )
    {
      send_to_char( "Drat, this one's got no zeni to start with.\n\r", ch );
      return;
    }
    victim->gold = 0;
    act( AT_MAGIC, "$n gestures and an unearthly gale sends $N's zeni flying!", ch, NULL, victim, TO_NOTVICT );
    act( AT_MAGIC, "You gesture and an unearthly gale sends $N's zeni flying!", ch, NULL, victim, TO_CHAR );
    act( AT_MAGIC, "As $n gestures, an unearthly gale sends your currency flying!", ch, NULL, victim, TO_VICT );
    return;
  }
  for( ;; )
  {
    pRoomIndex = get_room_index( number_range( 0, MAX_VNUMS ) );
    if( pRoomIndex )
      if( !xIS_SET( pRoomIndex->room_flags, ROOM_PRIVATE )
          && !xIS_SET( pRoomIndex->room_flags, ROOM_SOLITARY )
          && !xIS_SET( pRoomIndex->room_flags, ROOM_NO_ASTRAL ) && !xIS_SET( pRoomIndex->room_flags, ROOM_PROTOTYPE ) )
        break;
  }
  if( !str_cmp( arg2, "inventory" ) )
  {
    act( AT_MAGIC, "$n speaks a single word, sending $N's possessions flying!", ch, NULL, victim, TO_NOTVICT );
    act( AT_MAGIC, "You speak a single word, sending $N's possessions flying!", ch, NULL, victim, TO_CHAR );
    act( AT_MAGIC, "$n speaks a single word, sending your possessions flying!", ch, NULL, victim, TO_VICT );
    for( obj_lose = victim->first_carrying; obj_lose; obj_lose = obj_next )
    {
      /*
       * AREA_DATA *tarea = NULL;
       * for( tarea = first_area; tarea; tarea = tarea->next )
       * {
       * if( !str_cmp( tarea->planet->name, "earth" )
       * && number_range( 1, 100 ) < 5 )
       * {
       * break;
       * }
       * }
       */
      obj_next = obj_lose->next_content;
      obj_from_char( obj_lose );
      obj_to_room( obj_lose, pRoomIndex );
      pager_printf_color( ch, "\t&w%s sent to %d\n\r", capitalize( obj_lose->short_descr ), pRoomIndex->vnum );
    }
    return;
  }
  send_to_char( "Strew their coins or inventory?\n\r", ch );
  return;
}

void do_strip( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  OBJ_DATA *obj_next;
  OBJ_DATA *obj_lose;
  int count = 0;

  set_char_color( AT_OBJECT, ch );
  if( !argument )
  {
    send_to_char( "Strip who?\n\r", ch );
    return;
  }
  if( ( victim = get_char_room( ch, argument ) ) == NULL )
  {
    send_to_char( "They're not here.\n\r", ch );
    return;
  }
  if( victim == ch )
  {
    send_to_char( "Kinky.\n\r", ch );
    return;
  }
  if( !IS_NPC( victim ) && get_trust( victim ) >= get_trust( ch ) )
  {
    send_to_char( "You haven't the power to succeed against them.\n\r", ch );
    return;
  }
  act( AT_OBJECT, "Searching $N ...", ch, NULL, victim, TO_CHAR );
  for( obj_lose = victim->first_carrying; obj_lose; obj_lose = obj_next )
  {
    obj_next = obj_lose->next_content;
    obj_from_char( obj_lose );
    obj_to_char( obj_lose, ch );
    pager_printf_color( ch, "  &G... %s (&g%s) &Gtaken.\n\r", capitalize( obj_lose->short_descr ), obj_lose->name );
    count++;
  }
  if( !count )
    pager_printf_color( ch, "&GNothing found to take.\n\r", ch );
  return;
}

void do_restore( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  bool boost = FALSE;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Restore whom?\n\r", ch );
    return;
  }
  argument = one_argument( argument, arg2 );
  if( !str_cmp( arg2, "boost" ) && get_trust( ch ) >= LEVEL_SUB_IMPLEM )
  {
    send_to_char( "Boosting!\n\r", ch );
    boost = TRUE;
  }
  if( !str_cmp( arg, "all" ) )
  {
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    if( !ch->pcdata )
      return;

    if( get_trust( ch ) < LEVEL_SUB_IMPLEM )
    {
      if( IS_NPC( ch ) )
      {
        send_to_char( "You can't do that.\n\r", ch );
        return;
      }
      else
      {
        /*
         * Check if the player did a restore all within the last (RESTORE_INTERVAL/60)/60 time. Currently set to 4 hours 
	 * FIXED -- I've been noticing a) toka generally using restore all way too much.. b) toka used restore all when
	 * the only mortal online was his mortal, and c) this code was wrong and we needed to get that limit right.
         */
        if( !last_restore_all_time || 
	    (current_time - last_restore_all_time < RESTORE_INTERVAL) || 
	    (current_time - ch->pcdata->restore_time < RESTORE_INTERVAL) )
        {
          send_to_char( "Sorry, you can't do a restore all yet.\n\r", ch );
          do_restoretime( ch, "" );
          return;
        }
      }
    }
    last_restore_all_time = current_time;
    ch->pcdata->restore_time = current_time;
    save_char_obj( ch );
    send_to_char( "Beginning 'restore all' ...\n\r", ch );
    for( vch = first_char; vch; vch = vch_next )
    {
      vch_next = vch->next;

      if( !IS_NPC( vch ) )
      {
        if( boost )
          vch->hit = vch->max_hit * 1.5;
        else
          vch->hit = vch->max_hit;
        vch->mana = vch->max_mana;
        vch->move = vch->max_move;
        update_pos( vch );
        heart_calc( vch, "" );
        if( str_cmp( ch->name, "saiyr" ) )
          act( AT_IMMORT, "$n has restored you.", ch, NULL, vch, TO_VICT );
        else
          act( AT_LBLUE, "$n tosses soapy water on you, and you feel refreshed!", ch, NULL, vch, TO_VICT );
      }
    }
    send_to_char( "Restored.\n\r", ch );
  }
  else
  {

    CHAR_DATA *victim;

    if( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }

    if( get_trust( ch ) < LEVEL_LESSER && victim != ch && !( IS_NPC( victim ) && xIS_SET( victim->act, ACT_PROTOTYPE ) ) )
    {
      send_to_char( "You can't do that.\n\r", ch );
      return;
    }

    if( boost )
      victim->hit = victim->max_hit * 1.5;
    else
      victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    if( victim->pcdata )
      update_pos( victim );
    heart_calc( victim, "" );
    if( ch != victim )
      act( AT_IMMORT, "$n has restored you.", ch, NULL, victim, TO_VICT );
    send_to_char( "Restored.\n\r", ch );
    return;
  }
}

void do_restoretime( CHAR_DATA * ch, char *argument )
{
  long int time_passed;
  int hour, minute;

  set_char_color( AT_IMMORT, ch );

  if( !last_restore_all_time )
    ch_printf( ch, "There has been no restore all since reboot.\n\r" );
  else
  {
    time_passed = current_time - last_restore_all_time;
    hour = ( int )( time_passed / 3600 );
    minute = ( int )( ( time_passed - ( hour * 3600 ) ) / 60 );
    ch_printf( ch, "The last restore all was %d hours and %d minutes ago.\n\r", hour, minute );
  }

  if( !ch->pcdata )
    return;

  if( !ch->pcdata->restore_time )
  {
    send_to_char( "You have never done a restore all.\n\r", ch );
    return;
  }

  time_passed = current_time - ch->pcdata->restore_time;
  hour = ( int )( time_passed / 3600 );
  minute = ( int )( ( time_passed - ( hour * 3600 ) ) / 60 );
  ch_printf( ch, "Your last restore all was %d hours and %d minutes ago.\n\r", hour, minute );
  return;
}

void do_freeze( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_LBLUE, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Freeze whom?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }
  if( ch == victim )
  {
    send_to_char( "Don't freeze yourself you retard.\n\r", ch );
    return;
  }
  set_char_color( AT_LBLUE, victim );
  if( get_trust( victim ) >= get_trust( ch ) )
  {
    send_to_char( "You failed, and they saw...\n\r", ch );
    ch_printf( victim, "%s is attempting to freeze you.\n\r", ch->name );
    return;
  }
  if( xIS_SET( victim->act, PLR_FREEZE ) )
  {
    xREMOVE_BIT( victim->act, PLR_FREEZE );
    send_to_char( "Your frozen form suddenly thaws.\n\r", victim );
    ch_printf( ch, "%s is now unfrozen.\n\r", victim->name );
  }
  else
  {
    xSET_BIT( victim->act, PLR_FREEZE );
    send_to_char( "A godly force turns your body to ice!\n\r", victim );
    ch_printf( ch, "You have frozen %s.\n\r", victim->name );
  }
  save_char_obj( victim );
  return;
}

void do_log( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Log whom?\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "all" ) )
  {
    if( fLogAll )
    {
      fLogAll = FALSE;
      send_to_char( "Log ALL off.\n\r", ch );
    }
    else
    {
      fLogAll = TRUE;
      send_to_char( "Log ALL on.\n\r", ch );
    }
    return;
  }

  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }

  /*
   * No level check, gods can log anyone.
   */

  if( xIS_SET( victim->act, PLR_LOG ) )
  {
    xREMOVE_BIT( victim->act, PLR_LOG );
    ch_printf( ch, "LOG removed from %s.\n\r", victim->name );
  }
  else
  {
    xSET_BIT( victim->act, PLR_LOG );
    ch_printf( ch, "LOG applied to %s.\n\r", victim->name );
  }
  return;
}

void do_litterbug( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Set litterbug flag on whom?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }
  if( get_trust( victim ) >= get_trust( ch ) )
  {
    send_to_char( "You failed.\n\r", ch );
    return;
  }
  set_char_color( AT_IMMORT, victim );
  if( xIS_SET( victim->act, PLR_LITTERBUG ) )
  {
    xREMOVE_BIT( victim->act, PLR_LITTERBUG );
    send_to_char( "You can drop items again.\n\r", victim );
    ch_printf( ch, "LITTERBUG removed from %s.\n\r", victim->name );
  }
  else
  {
    xSET_BIT( victim->act, PLR_LITTERBUG );
    send_to_char( "A strange force prevents you from dropping any more items!\n\r", victim );
    ch_printf( ch, "LITTERBUG set on %s.\n\r", victim->name );
  }
  return;
}

void do_noemote( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Noemote whom?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }
  if( get_trust( victim ) >= get_trust( ch ) || ( IS_IMMORTAL( victim ) && get_trust( ch ) < 60 ) )
  {
    send_to_char( "You failed.\n\r", ch );
    return;
  }
  set_char_color( AT_IMMORT, victim );
  if( xIS_SET( victim->act, PLR_NO_EMOTE ) )
  {
    xREMOVE_BIT( victim->act, PLR_NO_EMOTE );
    send_to_char( "You can emote again.\n\r", victim );
    ch_printf( ch, "NOEMOTE removed from %s.\n\r", victim->name );
  }
  else
  {
    xSET_BIT( victim->act, PLR_NO_EMOTE );
    send_to_char( "You can't emote!\n\r", victim );
    ch_printf( ch, "NOEMOTE applied to %s.\n\r", victim->name );
  }
  return;
}

void do_notell( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Notell whom?", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }
  if( get_trust( victim ) >= get_trust( ch ) || ( IS_IMMORTAL( victim ) && get_trust( ch ) < 60 ) )
  {
    send_to_char( "You failed.\n\r", ch );
    return;
  }
  set_char_color( AT_IMMORT, victim );
  if( xIS_SET( victim->act, PLR_NO_TELL ) )
  {
    xREMOVE_BIT( victim->act, PLR_NO_TELL );
    send_to_char( "You can use tells again.\n\r", victim );
    ch_printf( ch, "NOTELL removed from %s.\n\r", victim->name );
  }
  else
  {
    xSET_BIT( victim->act, PLR_NO_TELL );
    send_to_char( "You can't use tells!\n\r", victim );
    ch_printf( ch, "NOTELL applied to %s.\n\r", victim->name );
  }
  return;
}

void do_notitle( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Notitle whom?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }
  if( get_trust( victim ) >= get_trust( ch ) || ( IS_IMMORTAL( victim ) && get_trust( ch ) < 60 ) )
  {
    send_to_char( "You failed.\n\r", ch );
    return;
  }
  set_char_color( AT_IMMORT, victim );
  if( IS_SET( victim->pcdata->flags, PCFLAG_NOTITLE ) )
  {
    REMOVE_BIT( victim->pcdata->flags, PCFLAG_NOTITLE );
    send_to_char( "You can set your own title again.\n\r", victim );
    ch_printf( ch, "NOTITLE removed from %s.\n\r", victim->name );
  }
  else
  {
    SET_BIT( victim->pcdata->flags, PCFLAG_NOTITLE );
    sprintf( buf, "the %s", title_table[victim->class][victim->level][victim->sex == SEX_FEMALE ? 1 : 0] );
    set_title( victim, buf );
    send_to_char( "You can't set your own title!\n\r", victim );
    ch_printf( ch, "NOTITLE set on %s.\n\r", victim->name );
  }
  return;
}

void do_silence( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int value;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Silence whom?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }
  if( get_trust( victim ) >= get_trust( ch ) || ( IS_IMMORTAL( victim ) && get_trust( ch ) < 60 ) )
  {
    send_to_char( "You failed.\n\r", ch );
    return;
  }
  if( argument[0] == '\0' )
  {
    send_to_char( "For how many minutes?\n\r", ch );
    return;
  }
  if( !is_number( argument ) )
  {
    send_to_char( "Those are not minutes...\n\r", ch );
    return;
  }
  set_char_color( AT_IMMORT, victim );

  if( xIS_SET( victim->act, PLR_SILENCE ) )
  {
    send_to_char( "Player already silenced, use unsilence to remove.\n\r", ch );
  }
  else
  {
    value = atoi( argument );
    if( value > 480 )
    {
      send_to_char( "I hope you know what your doing...\n\r", ch );
      sprintf( log_buf, "Log: %s silenced %s for more than 8 hours (%d minutes)", ch->name, victim->name, value );
      log_string_plus( log_buf, LOG_NORMAL, LEVEL_IMMORTAL );
    }
    sprintf( buf, "%s on %.24s", capitalize( ch->name ), ctime( &current_time ) );
    ch->pcdata->silencedby = STRALLOC( buf );
    xSET_BIT( victim->act, PLR_SILENCE );
    ch_printf( victim, "You can't use channels!  You have been silenced for %d minutes\n\r", value );
    ch_printf( ch, "You SILENCE %s.\n\r", victim->name );
    victim->pcdata->silence = value;
  }
  return;
}

              /*
               * Much better than toggling this with do_silence, yech --Blodkai 
               */
void do_unsilence( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Unsilence whom?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }
  if( get_trust( victim ) >= get_trust( ch ) )
  {
    send_to_char( "You failed.\n\r", ch );
    return;
  }
  set_char_color( AT_IMMORT, victim );
  if( xIS_SET( victim->act, PLR_SILENCE ) )
  {
    xREMOVE_BIT( victim->act, PLR_SILENCE );
    send_to_char( "You can use channels again.\n\r", victim );
    ch_printf( ch, "SILENCE removed from %s.\n\r", victim->name );
    victim->pcdata->silence = 0;
    STRFREE( ch->pcdata->silencedby );
  }
  else
  {
    send_to_char( "That player is not silenced.\n\r", ch );
  }
  return;
}

void do_peace( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *rch;

  act( AT_IMMORT, "$n booms, 'PEACE!'", ch, NULL, NULL, TO_ROOM );
  act( AT_IMMORT, "You boom, 'PEACE!'", ch, NULL, NULL, TO_CHAR );
  for( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
  {
    if( rch->fighting )
    {
      stop_fighting( rch, TRUE );
      do_sit( rch, "" );
    }

    /*
     * Added by Narn, Nov 28/95 
     */
    stop_hating( rch );
    stop_hunting( rch );
    stop_fearing( rch );
  }

  send_to_char_color( "&YOk.\n\r", ch );
  return;
}

WATCH_DATA *first_watch;
WATCH_DATA *last_watch;

void save_watchlist( void )
{
  WATCH_DATA *pwatch;
  FILE *fp;

  fclose( fpReserve );
  if( !( fp = fopen( SYSTEM_DIR WATCH_LIST, "w" ) ) )
  {
    bug( "Save_watchlist: Cannot open " WATCH_LIST, 0 );
    perror( WATCH_LIST );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }

  for( pwatch = first_watch; pwatch; pwatch = pwatch->next )
    fprintf( fp, "%d %s~%s~%s~\n", pwatch->imm_level, pwatch->imm_name,
             pwatch->target_name ? pwatch->target_name : " ", pwatch->player_site ? pwatch->player_site : " " );
  fprintf( fp, "-1\n" );
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
  return;
}

void do_wizlock( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  extern bool wizlock;
  extern int locklev;
  wizlock = !wizlock;
  int level_lock;

  set_char_color( AT_DANGER, ch );
  one_argument( argument, arg );
  if( !is_number( arg ) || arg[0] == '\0' )
    level_lock = 50;
  else
  {
    level_lock = atoi( arg );
  }
  locklev = level_lock;
  if( wizlock )
    send_to_char( "Game wizlocked.\n\r", ch );
  else
    send_to_char( "Game un-wizlocked.\n\r", ch );
  return;
}

void do_noresolve( CHAR_DATA * ch, char *argument )
{
  sysdata.NO_NAME_RESOLVING = !sysdata.NO_NAME_RESOLVING;

  if( sysdata.NO_NAME_RESOLVING )
    send_to_char_color( "&YName resolving disabled.\n\r", ch );
  else
    send_to_char_color( "&YName resolving enabled.\n\r", ch );
  return;
}

void do_users( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int count;
  char arg[MAX_INPUT_LENGTH];
  int mccpCount;
  float mccpPercent = 0;

  set_pager_color( AT_PLAIN, ch );

  one_argument( argument, arg );
  count = 0;
  mccpCount = 0;
  buf[0] = '\0';

  sprintf( buf, "\n\rDesc| Con|Idle|MCCP| Port | Player      @HostIP           " );
  if( get_trust( ch ) >= LEVEL_GOD )
    strcat( buf, "| Username" );
  strcat( buf, "\n\r" );
  strcat( buf, "----+----+----+----+------+-------------------------------" );
  if( get_trust( ch ) >= LEVEL_GOD )
    strcat( buf, "+---------" );
  strcat( buf, "\n\r" );
  send_to_pager( buf, ch );

  for( d = first_descriptor; d; d = d->next )
  {
    if( arg[0] == '\0' )
    {
      if( get_trust( ch ) >= LEVEL_SUPREME || ( d->character && can_see( ch, d->character ) ) )
      {
        count++;
        if( d->compressing )
          mccpCount++;
        sprintf( buf,
                 " %3d| %3d|%4d|%4d|%6d| %-12s@%-16s ",
                 d->descriptor,
                 d->connected,
                 d->idle / 4,
                 d->compressing,
                 d->port, d->original ? d->original->name : d->character ? d->character->name : "(none)", d->host );
        if( get_trust( ch ) >= LEVEL_GOD )
          sprintf( buf + strlen( buf ), "| %s", d->user );
        strcat( buf, "\n\r" );
        send_to_pager( buf, ch );
      }
    }
    else
    {
      if( ( get_trust( ch ) >= LEVEL_SUPREME
            || ( d->character && can_see( ch, d->character ) ) )
          && ( !str_prefix( arg, d->host ) || ( d->character && !str_prefix( arg, d->character->name ) ) ) )
      {
        count++;
        if( d->compressing )
          mccpCount++;
        pager_printf( ch,
                      " %3d| %3d|%4d|%4d|%6d| %-12s@%-16s ",
                      d->descriptor,
                      d->connected,
                      d->idle / 4,
                      d->compressing,
                      d->port, d->original ? d->original->name : d->character ? d->character->name : "(none)", d->host );
        buf[0] = '\0';
        if( get_trust( ch ) >= LEVEL_GOD )
          sprintf( buf, "| %s", d->user );
        strcat( buf, "\n\r" );
        send_to_pager( buf, ch );
      }
    }
  }
  mccpPercent = ( float )mccpCount / count * 100;
  pager_printf( ch, "%d user%s.\n\r", count, count == 1 ? "" : "s" );
  pager_printf( ch, "%d(%.0f%%) MCCP user%s.\n\r", mccpCount, mccpPercent, mccpCount == 1 ? "" : "s" );
  return;
}

/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  bool mobsonly;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( arg[0] == '\0' || argument[0] == '\0' )
  {
    send_to_char( "Force whom to do what?\n\r", ch );
    return;
  }

  mobsonly = get_trust( ch ) < sysdata.level_forcepc;

  if( !str_cmp( arg, "all" ) )
  {
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    if( mobsonly )
    {
      send_to_char( "Force whom to do what?\n\r", ch );
      return;
    }

    for( vch = first_char; vch; vch = vch->next )
    {
      if( vch->tmystic > 0 )
      {
        ch_printf( ch, "&R%s is teaching mystic.\n\r", vch->name );
        ch_printf( ch, "&RForce Failed.\n\r" );
        return;
      }
    }

    if( str_cmp( argument, "save" ) && str_cmp( argument, "quit" ) && ch->level < 63 )
    {
      ch_printf( ch, "Access denied.\n\r" );
      return;
    }

    for( vch = first_char; vch; vch = vch_next )
    {
      vch_next = vch->next;

      if( !IS_NPC( vch ) && get_trust( vch ) < get_trust( ch ) )
      {
        act( AT_IMMORT, "$n forces you to '$t'.", ch, argument, vch, TO_VICT );
        interpret( vch, argument );
      }
    }
  }
  else
  {
    CHAR_DATA *victim;

    if( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }

    if( victim == ch )
    {
      send_to_char( "Aye aye, right away!\n\r", ch );
      return;
    }

    if( mobsonly && IS_NPC( victim ) && !xIS_SET( victim->act, ACT_PROTOTYPE )
        && get_trust( ch ) < sysdata.level_modify_proto )
    {
      send_to_char( "You don't have the power to do that.\n\r", ch );
      return;
    }

    if( ( get_trust( victim ) >= get_trust( ch ) ) || ( mobsonly && !IS_NPC( victim ) ) )
    {
      send_to_char( "Do it yourself!\n\r", ch );
      return;
    }

    if( get_trust( ch ) < LEVEL_GOD && IS_NPC( victim ) && !str_prefix( "mp", argument ) )
    {
      send_to_char( "You can't force a mob to do that!\n\r", ch );
      return;
    }
    act( AT_IMMORT, "$n forces you to '$t'.", ch, argument, victim, TO_VICT );
    interpret( victim, argument );
  }

  send_to_char( "Ok.\n\r", ch );
  return;
}



//lssj project
void do_addlessj( CHAR_DATA * ch, char *argument )
{
  /*
   * char arg[MAX_STRING_LENGTH];
   */
  CHAR_DATA *vch;

  if( ch->level < 63 )
  {
    send_to_char( "Your not allowed to teach lssj.\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) )
    return;



  if( ( vch = get_char_room( ch, argument ) ) == NULL )
  {
    ch_printf( ch, "There's nobody here by that name.\n\r" );
    return;
  }



  if( IS_NPC( vch ) )
  {
    ch_printf( ch, "Not on NPC's.\n\r" );
    return;
  }
  if( vch->exp < 100000000000ULL )
  {
    ch_printf( ch, "They have to be over 100bil to learn lssj.\n\r" );
    return;
  }
/* Karma only addressed Androids. I fixed. -Karn 02.08.05 */
  if( is_saiyan( vch ) )
    if( is_saiyan_s( vch ) )
      if( is_saiyan_h( vch ) )
        if( is_saiyan_n( vch ) )
          if( is_saiyan_hb( vch ) )
          {
            ch_printf( ch, "They cannot be taught lssj.\n\r" );
            return;
          }

  if( vch->position != POS_STANDING )
  {
    ch_printf( ch, "They have to be standing.\n\r" );
    return;
  }
  if( IS_SET( vch->pcdata->flags, PCFLAG_KNOWSLSSJ ) )
  {
    ch_printf( ch, "%s already knows lssj.\n\r", vch->name );
    return;
  }

  if( IS_SET( vch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
  {
    ch_printf( ch, "%s already knows another way of fighting.\n\r", vch->name );
    return;
  }

  ch_printf( ch, "\n\r" );
  ch_printf( vch, "\n\r" );
  act( AT_YELLOW,
       "You are putting your hands upon $N's head as your hands suddnely begins to glow. Your hands glowing wears off. You can sense great power within $N",
       ch, NULL, vch, TO_CHAR );
  act( AT_YELLOW,
       "$n putting $s hands upon your head as $s hands suddnely begins to glow. $n's hands glowing wears off. $n can sense great power within you",
       ch, NULL, vch, TO_VICT );
  act( AT_YELLOW,
       "$n putting $s hands upon $N's head as $s hands suddnely begins to glow. $n's hands glowing wears off. you can sense great power within $N",
       ch, NULL, vch, TO_NOTVICT );

  vch->pcdata->learned[gsn_lssj] = 10;
  SET_BIT( vch->pcdata->flags, PCFLAG_KNOWSLSSJ );
  save_char_obj( vch );

  WAIT_STATE( vch, skill_table[gsn_lssj]->beats );

  do_lssj( vch, "30" );
  return;
}



void do_invis( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  sh_int level;

  set_char_color( AT_IMMORT, ch );

  /*
   * if ( IS_NPC(ch)) return; 
   */

  argument = one_argument( argument, arg );
  if( arg && arg[0] != '\0' )
  {
    if( !is_number( arg ) )
    {
      send_to_char( "Usage: invis | invis <level>\n\r", ch );
      return;
    }
    level = atoi( arg );
    if( level < 2 || level > get_trust( ch ) )
    {
      send_to_char( "Invalid level.\n\r", ch );
      return;
    }

    if( !IS_NPC( ch ) )
    {
      ch->pcdata->wizinvis = level;
      ch_printf( ch, "Wizinvis level set to %d.\n\r", level );
    }

    if( IS_NPC( ch ) )
    {
      ch->mobinvis = level;
      ch_printf( ch, "Mobinvis level set to %d.\n\r", level );
    }
    return;
  }

  if( !IS_NPC( ch ) )
  {
    if( ch->pcdata->wizinvis < 2 )
      ch->pcdata->wizinvis = ch->level;
  }
  if( IS_NPC( ch ) )
  {
    if( ch->mobinvis < 2 )
      ch->mobinvis = ch->level;
  }
  if( xIS_SET( ch->act, PLR_WIZINVIS ) )
  {
    xREMOVE_BIT( ch->act, PLR_WIZINVIS );
    act( AT_IMMORT, "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You slowly fade back into existence.\n\r", ch );
  }
  else
  {
    act( AT_IMMORT, "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You slowly vanish into thin air.\n\r", ch );
    xSET_BIT( ch->act, PLR_WIZINVIS );
  }
  return;
}


void do_holylight( CHAR_DATA * ch, char *argument )
{

  set_char_color( AT_IMMORT, ch );

  if( IS_NPC( ch ) )
    return;

  if( xIS_SET( ch->act, PLR_HOLYLIGHT ) )
  {
    xREMOVE_BIT( ch->act, PLR_HOLYLIGHT );
    send_to_char( "Holy light mode off.\n\r", ch );
  }
  else
  {
    xSET_BIT( ch->act, PLR_HOLYLIGHT );
    send_to_char( "Holy light mode on.\n\r", ch );
  }
  return;
}

void do_rassign( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  int r_lo, r_hi;
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  send_to_char( "Please use 'vassign'.\n\r", ch );
  return;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );
  r_lo = atoi( arg2 );
  r_hi = atoi( arg3 );

  if( arg1[0] == '\0' || r_lo < 0 || r_hi < 0 )
  {
    send_to_char( "Syntax: rassign <who> <low> <high>\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
  {
    send_to_char( "They don't seem to be around.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) || get_trust( victim ) < LEVEL_CREATOR )
  {
    send_to_char( "They wouldn't know what to do with a room range.\n\r", ch );
    return;
  }
  if( r_lo > r_hi )
  {
    send_to_char( "Unacceptable room range.\n\r", ch );
    return;
  }
  if( r_lo == 0 )
    r_hi = 0;
  victim->pcdata->r_range_lo = r_lo;
  victim->pcdata->r_range_hi = r_hi;
  assign_area( victim );
  send_to_char( "Done.\n\r", ch );
  set_char_color( AT_IMMORT, victim );
  ch_printf( victim, "%s has assigned you the room vnum range %d - %d.\n\r", ch->name, r_lo, r_hi );
  assign_area( victim );  /* Put back by Thoric on 02/07/96 */
  if( !victim->pcdata->area )
  {
    bug( "rassign: assign_area failed", 0 );
    return;
  }

  if( r_lo == 0 ) /* Scryn 8/12/95 */
  {
    REMOVE_BIT( victim->pcdata->area->status, AREA_LOADED );
    SET_BIT( victim->pcdata->area->status, AREA_DELETED );
  }
  else
  {
    SET_BIT( victim->pcdata->area->status, AREA_LOADED );
    REMOVE_BIT( victim->pcdata->area->status, AREA_DELETED );
  }
  return;
}

void do_oassign( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  int o_lo, o_hi;
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  send_to_char( "Please use 'vassign'.\n\r", ch );
  return;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );
  o_lo = atoi( arg2 );
  o_hi = atoi( arg3 );

  if( arg1[0] == '\0' || o_lo < 0 || o_hi < 0 )
  {
    send_to_char( "Syntax: oassign <who> <low> <high>\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
  {
    send_to_char( "They don't seem to be around.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) || get_trust( victim ) < LEVEL_SAVIOR )
  {
    send_to_char( "They wouldn't know what to do with an object range.\n\r", ch );
    return;
  }
  if( o_lo > o_hi )
  {
    send_to_char( "Unacceptable object range.\n\r", ch );
    return;
  }
  victim->pcdata->o_range_lo = o_lo;
  victim->pcdata->o_range_hi = o_hi;
  assign_area( victim );
  send_to_char( "Done.\n\r", ch );
  set_char_color( AT_IMMORT, victim );
  ch_printf( victim, "%s has assigned you the object vnum range %d - %d.\n\r", ch->name, o_lo, o_hi );
  return;
}

void do_massign( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  int m_lo, m_hi;
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  send_to_char( "Please use 'vassign'.\n\r", ch );
  return;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );
  m_lo = atoi( arg2 );
  m_hi = atoi( arg3 );

  if( arg1[0] == '\0' || m_lo < 0 || m_hi < 0 )
  {
    send_to_char( "Syntax: massign <who> <low> <high>\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
  {
    send_to_char( "They don't seem to be around.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) || get_trust( victim ) < LEVEL_SAVIOR )
  {
    send_to_char( "They wouldn't know what to do with a monster range.\n\r", ch );
    return;
  }
  if( m_lo > m_hi )
  {
    send_to_char( "Unacceptable monster range.\n\r", ch );
    return;
  }
  victim->pcdata->m_range_lo = m_lo;
  victim->pcdata->m_range_hi = m_hi;
  assign_area( victim );
  send_to_char( "Done.\n\r", ch );
  set_char_color( AT_IMMORT, victim );
  ch_printf( victim, "%s has assigned you the monster vnum range %d - %d.\n\r", ch->name, m_lo, m_hi );
  return;
}

void do_cmdtable( CHAR_DATA * ch, char *argument )
{
  int hash, cnt;
  CMDTYPE *cmd;
  char arg[MAX_INPUT_LENGTH];

  one_argument( argument, arg );

  if( strcmp( arg, "lag" ) )  /* display normal command table */
  {
    set_pager_color( AT_IMMORT, ch );
    send_to_pager( "Commands and Number of Uses This Run\n\r", ch );
    set_pager_color( AT_PLAIN, ch );
    for( cnt = hash = 0; hash < 126; hash++ )
      for( cmd = command_hash[hash]; cmd; cmd = cmd->next )
      {
        if( ( ++cnt ) % 4 )
          pager_printf( ch, "%-6.6s %4d\t", cmd->name, cmd->userec.num_uses );
        else
          pager_printf( ch, "%-6.6s %4d\n\r", cmd->name, cmd->userec.num_uses );
      }
    send_to_char( "\n\r", ch );
  }
  else  /* display commands causing lag */
  {
    set_pager_color( AT_IMMORT, ch );
    send_to_pager( "Commands that have caused lag this run\n\r", ch );
    set_pager_color( AT_PLAIN, ch );
    for( cnt = hash = 0; hash < 126; hash++ )
      for( cmd = command_hash[hash]; cmd; cmd = cmd->next )
      {
        if( !cmd->lag_count )
          continue;
        else if( ( ++cnt ) % 4 )
          pager_printf( ch, "%-6.6s %4d\t", cmd->name, cmd->lag_count );
        else
          pager_printf( ch, "%-6.6s %4d\n\r", cmd->name, cmd->lag_count );
      }
    send_to_char( "\n\r", ch );
  }

  return;
}

void do_mortalize( CHAR_DATA * ch, char *argument )
{
  char fname[1024];
  char name[256];
  struct stat fst;
  bool loaded;
  DESCRIPTOR_DATA *d;
  int old_room_vnum;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  AREA_DATA *pArea;
  int sn;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, name );
  if( name[0] == '\0' )
  {
    send_to_char( "Usage: mortalize <playername>\n\r", ch );
    return;
  }

  name[0] = UPPER( name[0] );
  sprintf( fname, "%s%c/%s", PLAYER_DIR, tolower( name[0] ), capitalize( name ) );
  if( stat( fname, &fst ) != -1 )
  {
    CREATE( d, DESCRIPTOR_DATA, 1 );
    d->next = NULL;
    d->prev = NULL;
    d->connected = CON_GET_NAME;
    d->outsize = 2000;
    CREATE( d->outbuf, char, d->outsize );

    loaded = load_char_obj( d, name, FALSE );
    add_char( d->character );
    old_room_vnum = d->character->in_room->vnum;
    char_to_room( d->character, ch->in_room );
    if( get_trust( d->character ) >= get_trust( ch ) )
    {
      do_say( d->character, "Do *NOT* disturb me again!" );
      send_to_char( "I think you'd better leave that player alone!\n\r", ch );
      d->character->desc = NULL;
      do_quit( d->character, "" );
      return;
    }
    d->character->desc = NULL;
    victim = d->character;
    d->character = NULL;
    DISPOSE( d->outbuf );
    DISPOSE( d );
    victim->level = 50;
    //        victim->exp      = exp_level(victim, 50);
    victim->max_hit = 100;
    victim->max_mana = 800;
    victim->max_move = 800;
    for( sn = 0; sn < top_sn; sn++ )
      victim->pcdata->learned[sn] = 0;
    victim->practice = 0;
    victim->train = 0;
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    advance_level( victim );
    if( victim->pcdata->pretitle )
      STRFREE( victim->pcdata->pretitle );
    victim->pcdata->pretitle = str_dup( "" );
    if( xIS_SET( victim->act, PLR_WIZINVIS ) )
      victim->pcdata->wizinvis = 0;
    if( xIS_SET( victim->act, PLR_WIZINVIS ) && ( victim->level <= LEVEL_AVATAR ) )
    {
      xREMOVE_BIT( victim->act, PLR_WIZINVIS );
      victim->pcdata->wizinvis = 0;
    }
    sprintf( buf, "%s%s", GOD_DIR, capitalize( victim->name ) );

    if( !remove( buf ) )
      send_to_char( "Player's immortal data destroyed.\n\r", ch );
    else if( errno != ENOENT )
    {
      ch_printf( ch, "Unknown error #%d - %s (immortal data).  Report to Thoric\n\r", errno, strerror( errno ) );
      sprintf( buf2, "%s mortalizing %s", ch->name, buf );
      perror( buf2 );
    }
    sprintf( buf2, "%s.are", capitalize( argument ) );
    for( pArea = first_build; pArea; pArea = pArea->next )
      if( !strcmp( pArea->filename, buf2 ) )
      {
        sprintf( buf, "%s%s", BUILD_DIR, buf2 );
        if( IS_SET( pArea->status, AREA_LOADED ) )
          fold_area( pArea, buf, FALSE );
        close_area( pArea );
        sprintf( buf2, "%s.bak", buf );
        set_char_color( AT_RED, ch );
        if( !rename( buf, buf2 ) )
          send_to_char( "Player's area data destroyed.  Area saved as backup.\n\r", ch );
        else if( errno != ENOENT )
        {
          ch_printf( ch, "Unknown error #%d - %s (area data).  Report to Thoric.\n\r", errno, strerror( errno ) );
          sprintf( buf2, "%s mortalizing %s", ch->name, buf );
          perror( buf2 );
        }
      }
    make_wizlist(  );
    while( victim->first_carrying )
      extract_obj( victim->first_carrying );
    do_quit( victim, "" );
    return;
  }
  send_to_char( "No such player.\n\r", ch );
  return;
}

/*
 * Load up a player file
 */
void do_loadup( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *temp;
  char fname[1024];
  char name[256];
  struct stat fst;
  bool loaded;
  DESCRIPTOR_DATA *d;
  int old_room_vnum;
  char buf[MAX_STRING_LENGTH];

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, name );
  if( name[0] == '\0' )
  {
    send_to_char( "Usage: loadup <playername>\n\r", ch );
    return;
  }
  for( temp = first_char; temp; temp = temp->next )
  {
    if( IS_NPC( temp ) )
      continue;
    if( can_see( ch, temp ) && !str_cmp( name, temp->name ) )
      break;
  }
  if( temp != NULL )
  {
    send_to_char( "They are already playing.\n\r", ch );
    return;
  }
  name[0] = UPPER( name[0] );
  sprintf( fname, "%s%c/%s", PLAYER_DIR, tolower( name[0] ), capitalize( name ) );

  if( stat( fname, &fst ) != -1 )
  {
    CREATE( d, DESCRIPTOR_DATA, 1 );
    d->next = NULL;
    d->prev = NULL;
    d->connected = CON_GET_NAME;
    d->outsize = 2000;
    CREATE( d->outbuf, char, d->outsize );

    loaded = load_char_obj( d, name, FALSE );
    add_char( d->character );
    old_room_vnum = d->character->in_room->vnum;
    char_to_room( d->character, ch->in_room );
    if( get_trust( d->character ) >= get_trust( ch ) )
    {
      do_say( d->character, "Do *NOT* disturb me again!" );
      send_to_char( "I think you'd better leave that player alone!\n\r", ch );
      d->character->desc = NULL;
      do_quit( d->character, "" );
      return;
    }
    d->character->desc = NULL;
    d->character->retran = old_room_vnum;
    d->character = NULL;
    DISPOSE( d->outbuf );
    DISPOSE( d );
    ch_printf( ch, "Player %s loaded from room %d.\n\r", capitalize( name ), old_room_vnum );
    sprintf( buf, "%s appears from nowhere, eyes glazed over.\n\r", capitalize( name ) );
    act( AT_IMMORT, buf, ch, NULL, NULL, TO_ROOM );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  /*
   * else no player file 
   */
  send_to_char( "No such player.\n\r", ch );
  return;
}

void do_fixchar( CHAR_DATA * ch, char *argument )
{
  char name[MAX_STRING_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, name );
  if( name[0] == '\0' )
  {
    send_to_char( "Usage: fixchar <playername>\n\r", ch );
    return;
  }

  victim = get_char_room( ch, name );
  if( !victim )
  {
    send_to_char( "They're not here.\n\r", ch );
    return;
  }
  fix_char( victim );
  /*
   * victim->armor  = 100;
   * victim->mod_str  = 0;
   * victim->mod_dex  = 0;
   * victim->mod_wis  = 0;
   * victim->mod_int  = 0;
   * victim->mod_con  = 0;
   * victim->mod_cha  = 0;
   * victim->mod_lck  = 0;
   * victim->damroll  = 0;
   * victim->hitroll  = 0;
   * victim->alignment  = URANGE( -1000, victim->alignment, 1000 );
   * victim->saving_spell_staff = 0; 
   */
  send_to_char( "Done.\n\r", ch );
}

void do_newbieset( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  //    OBJ_DATA *obj;
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if( arg1[0] == '\0' )
  {
    send_to_char( "Syntax: newbieset <char>.\n\r", ch );
    return;
  }
  if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    send_to_char( "That player is not here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }
  if( ( victim->level < 1 ) || ( victim->level > 5 ) )
  {
    send_to_char( "Level of victim must be between 1 and 5.\n\r", ch );
    return;
  }


  act( AT_IMMORT, "$n has equipped you with a newbieset.", ch, NULL, victim, TO_VICT );
  ch_printf( ch, "You have re-equipped %s.\n\r", victim->name );
  return;
}

/*
 * Extract area names from "input" string and place result in "output" string
 * e.g. "aset joe.are sedit susan.are cset" --> "joe.are susan.are"
 * - Gorog
 */
void extract_area_names( char *inp, char *out )
{
  char buf[MAX_INPUT_LENGTH], *pbuf = buf;
  int len;

  *out = '\0';
  while( inp && *inp )
  {
    inp = one_argument( inp, buf );
    if( ( len = strlen( buf ) ) >= 5 && !strcmp( ".are", pbuf + len - 4 ) )
    {
      if( *out )
        strcat( out, " " );
      strcat( out, buf );
    }
  }
}

/*
 * Remove area names from "input" string and place result in "output" string
 * e.g. "aset joe.are sedit susan.are cset" --> "aset sedit cset"
 * - Gorog
 */
void remove_area_names( char *inp, char *out )
{
  char buf[MAX_INPUT_LENGTH], *pbuf = buf;
  int len;

  *out = '\0';
  while( inp && *inp )
  {
    inp = one_argument( inp, buf );
    if( ( len = strlen( buf ) ) < 5 || strcmp( ".are", pbuf + len - 4 ) )
    {
      if( *out )
        strcat( out, " " );
      strcat( out, buf );
    }
  }
}

/*
 * Allows members of the Area Council to add Area names to the bestow field.
 * Area names mus end with ".are" so that no commands can be bestowed.
 */
void do_bestowarea( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int arg_len;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );

  if( !*arg )
  {
    send_to_char( "Syntax:\n\r"
                  "bestowarea <victim> <filename>.are\n\r"
                  "bestowarea <victim> none             removes bestowed areas\n\r"
                  "bestowarea <victim> list             lists bestowed areas\n\r"
                  "bestowarea <victim>                  lists bestowed areas\n\r", ch );
    return;
  }
  if( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "You can't give special abilities to a mob!\n\r", ch );
    return;
  }
  if( get_trust( victim ) < LEVEL_IMMORTAL )
  {
    send_to_char( "They aren't an immortal.\n\r", ch );
    return;
  }

  if( !victim->pcdata->bestowments )
    victim->pcdata->bestowments = str_dup( "" );

  if( !*argument || !str_cmp( argument, "list" ) )
  {
    extract_area_names( victim->pcdata->bestowments, buf );
    ch_printf( ch, "Bestowed areas: %s\n\r", buf );
    return;
  }
  if( !str_cmp( argument, "none" ) )
  {
    remove_area_names( victim->pcdata->bestowments, buf );
    DISPOSE( victim->pcdata->bestowments );
    victim->pcdata->bestowments = str_dup( buf );
    send_to_char( "Done.\n\r", ch );
    return;
  }

  arg_len = strlen( argument );
  if( arg_len < 5
      || argument[arg_len - 4] != '.' || argument[arg_len - 3] != 'a'
      || argument[arg_len - 2] != 'r' || argument[arg_len - 1] != 'e' )
  {
    send_to_char( "You can only bestow an area name\n\r", ch );
    send_to_char( "E.G. bestow joe sam.are\n\r", ch );
    return;
  }

  sprintf( buf, "%s %s", victim->pcdata->bestowments, argument );
  DISPOSE( victim->pcdata->bestowments );
  victim->pcdata->bestowments = str_dup( buf );
  set_char_color( AT_IMMORT, victim );
  ch_printf( victim, "%s has bestowed on you the area: %s\n\r", ch->name, argument );
  send_to_char( "Done.\n\r", ch );
}

void do_bestow( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Bestow whom with what?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "You can't give special abilities to a mob!\n\r", ch );
    return;
  }
  if( get_trust( victim ) > get_trust( ch ) )
  {
    send_to_char( "You aren't powerful enough...\n\r", ch );
    return;
  }

  if( !victim->pcdata->bestowments )
    victim->pcdata->bestowments = str_dup( "" );

  if( argument[0] == '\0' || !str_cmp( argument, "list" ) )
  {
    ch_printf( ch, "Current bestowed commands on %s: %s.\n\r", victim->name, victim->pcdata->bestowments );
    return;
  }
  if( !str_cmp( argument, "none" ) )
  {
    DISPOSE( victim->pcdata->bestowments );
    victim->pcdata->bestowments = str_dup( "" );
    ch_printf( ch, "Bestowments removed from %s.\n\r", victim->name );
    ch_printf( victim, "%s has removed your bestowed commands.\n\r", ch->name );
    return;
  }

  sprintf( buf, "%s %s", victim->pcdata->bestowments, argument );
  DISPOSE( victim->pcdata->bestowments );
  victim->pcdata->bestowments = str_dup( buf );
  set_char_color( AT_IMMORT, victim );
  ch_printf( victim, "%s has bestowed on you the command(s): %s\n\r", ch->name, argument );
  send_to_char( "Done.\n\r", ch );
}

struct tm *update_time( struct tm *old_time )
{
  time_t time;

  time = mktime( old_time );
  return localtime( &time );
}

void do_settime( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  if( ch->level < 63 || IS_NPC( ch ) )
  {
    ch_printf( ch, "Huh?!?\n\r" );
    return;
  }
  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg2 );
  if( arg[0] == '\0' || arg2[0] == '\0' )
  {
    ch_printf( ch, "Syntax: settime (hour/day) (#)\n\r" );
    return;
  }
  if( !str_cmp( arg, "hour" ) )
  {
    int a = atoi( arg2 );
    if( a < 1 || a > 24 )
    {
      ch_printf( ch, "Hour must be a number from 1 to 24.\n\r" );
      return;
    }
    time_info.hour = a;
    ch_printf( ch, "Done.\n\r" );
    return;
  }
  if( !str_cmp( arg, "day" ) )
  {
    int a = atoi( arg2 );
    if( a < 1 || a > 30 )
    {
      ch_printf( ch, "Day must be a number from 1 to 30.\n\r" );
      return;
    }
    time_info.day = a;
    ch_printf( ch, "Done.\n\r" );
    return;
  }
  do_settime( ch, "" );
}

void do_set_boot_time( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  bool check;

  check = FALSE;
  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Syntax: setboot time {hour minute <day> <month> <year>}\n\r", ch );
    send_to_char( "        setboot manual {0/1}\n\r", ch );
    send_to_char( "        setboot default\n\r", ch );
    ch_printf( ch, "Boot time is currently set to %s, manual bit is set to %d\n\r", reboot_time, set_boot_time->manual );
    return;
  }

  if( !str_cmp( arg, "time" ) )
  {
    struct tm *now_time;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );
    if( !*arg || !*arg1 || !is_number( arg ) || !is_number( arg1 ) )
    {
      send_to_char( "You must input a value for hour and minute.\n\r", ch );
      return;
    }

    now_time = localtime( &current_time );
    if( ( now_time->tm_hour = atoi( arg ) ) < 0 || now_time->tm_hour > 23 )
    {
      send_to_char( "Valid range for hour is 0 to 23.\n\r", ch );
      return;
    }
    if( ( now_time->tm_min = atoi( arg1 ) ) < 0 || now_time->tm_min > 59 )
    {
      send_to_char( "Valid range for minute is 0 to 59.\n\r", ch );
      return;
    }

    argument = one_argument( argument, arg );
    if( *arg != '\0' && is_number( arg ) )
    {
      if( ( now_time->tm_mday = atoi( arg ) ) < 1 || now_time->tm_mday > 31 )
      {
        send_to_char( "Valid range for day is 1 to 31.\n\r", ch );
        return;
      }
      argument = one_argument( argument, arg );
      if( *arg != '\0' && is_number( arg ) )
      {
        if( ( now_time->tm_mon = atoi( arg ) ) < 1 || now_time->tm_mon > 12 )
        {
          send_to_char( "Valid range for month is 1 to 12.\n\r", ch );
          return;
        }
        now_time->tm_mon--;
        argument = one_argument( argument, arg );
        if( ( now_time->tm_year = atoi( arg ) - 1900 ) < 0 || now_time->tm_year > 199 )
        {
          send_to_char( "Valid range for year is 1900 to 2099.\n\r", ch );
          return;
        }
      }
    }

    now_time->tm_sec = 0;
    if( mktime( now_time ) < current_time )
    {
      send_to_char( "You can't set a time previous to today!\n\r", ch );
      return;
    }
    if( set_boot_time->manual == 0 )
      set_boot_time->manual = 1;
    new_boot_time = update_time( now_time );
    new_boot_struct = *new_boot_time;
    new_boot_time = &new_boot_struct;
    reboot_check( mktime( new_boot_time ) );
    get_reboot_string(  );

    ch_printf( ch, "Boot time set to %s\n\r", reboot_time );
    check = TRUE;
  }
  else if( !str_cmp( arg, "manual" ) )
  {
    argument = one_argument( argument, arg1 );
    if( arg1[0] == '\0' )
    {
      send_to_char( "Please enter a value for manual boot on/off\n\r", ch );
      return;
    }
    if( !is_number( arg1 ) )
    {
      send_to_char( "Value for manual must be 0 (off) or 1 (on)\n\r", ch );
      return;
    }
    if( atoi( arg1 ) < 0 || atoi( arg1 ) > 1 )
    {
      send_to_char( "Value for manual must be 0 (off) or 1 (on)\n\r", ch );
      return;
    }

    set_boot_time->manual = atoi( arg1 );
    ch_printf( ch, "Manual bit set to %s\n\r", arg1 );
    check = TRUE;
    get_reboot_string(  );
    return;
  }

  else if( !str_cmp( arg, "default" ) )
  {
    set_boot_time->manual = 0;
    /*
     * Reinitialize new_boot_time 
     */
    new_boot_time = localtime( &current_time );
/*		new_boot_time->tm_mday += 1;
		if (new_boot_time->tm_hour > 12)
			new_boot_time->tm_mday += 1;
*/ new_boot_time->tm_hour = 4;
    new_boot_time->tm_min = 0;
    new_boot_time->tm_sec = 0;
    new_boot_time = update_time( new_boot_time );

    sysdata.DENY_NEW_PLAYERS = FALSE;

    send_to_char( "Reboot time set back to normal.\n\r", ch );
    check = TRUE;
  }

  if( !check )
  {
    send_to_char( "Invalid argument for setboot.\n\r", ch );
    return;
  }
  else
  {
    get_reboot_string(  );
    new_boot_time_t = mktime( new_boot_time );
  }
}

/* Online high level immortal command for displaying what the encryption
 * of a name/password would be, taking in 2 arguments - the name and the
 * password - can still only change the password if you have access to
 * pfiles and the correct password
 */
void do_form_password( CHAR_DATA * ch, char *argument )
{
  char *pwcheck, *p;

  set_char_color( AT_IMMORT, ch );

  if( !argument || argument[0] == '\0' )
  {
    send_to_char( "Usage: formpass <password>\n\r", ch );
    return;
  }

  /*
   * This is arbitrary to discourage weak passwords 
   */
  if( strlen( argument ) < 5 )
  {
    send_to_char( "Usage: formpass <password>\n\r", ch );
    send_to_char( "New password must be at least 5 characters in length.\n\r", ch );
    return;
  }

  if( argument[0] == '!' )
  {
    send_to_char( "Usage: formpass <password>\n\r", ch );
    send_to_char( "New password cannot begin with the '!' character.\n\r", ch );
    return;
  }

  pwcheck = smaug_crypt( argument );
  for( p = pwcheck; *p != '\0'; p++ )
  {
    if( *p == '~' )
    {
      send_to_char( "New password not acceptable, cannot use the ~ character.\n\r", ch );
      return;
    }
  }
  ch_printf( ch, "%s results in the encrypted string: %s\n\r", argument, pwcheck );
  return;
}

/*
 * Purge a player file.  No more player.  -- Altrag
 */
void do_destro( CHAR_DATA * ch, char *argument )
{
  set_char_color( AT_RED, ch );
  send_to_char( "If you want to destroy a character, spell it out!\n\r", ch );
  return;
}

/*
 * This could have other applications too.. move if needed. -- Altrag
 */
void close_area( AREA_DATA * pArea )
{
  extern ROOM_INDEX_DATA *room_index_hash[MAX_KEY_HASH];
  extern OBJ_INDEX_DATA *obj_index_hash[MAX_KEY_HASH];
  extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
  CHAR_DATA *ech;
  CHAR_DATA *ech_next;
  OBJ_DATA *eobj;
  OBJ_DATA *eobj_next;
  int icnt;
  ROOM_INDEX_DATA *rid;
  ROOM_INDEX_DATA *rid_next;
  OBJ_INDEX_DATA *oid;
  OBJ_INDEX_DATA *oid_next;
  MOB_INDEX_DATA *mid;
  MOB_INDEX_DATA *mid_next;
  RESET_DATA *ereset;
  RESET_DATA *ereset_next;
  EXTRA_DESCR_DATA *eed;
  EXTRA_DESCR_DATA *eed_next;
  EXIT_DATA *exit;
  EXIT_DATA *exit_next;
  MPROG_ACT_LIST *mpact;
  MPROG_ACT_LIST *mpact_next;
  MPROG_DATA *mprog;
  MPROG_DATA *mprog_next;
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;

  for( ech = first_char; ech; ech = ech_next )
  {
    ech_next = ech->next;

    if( ech->fighting )
      stop_fighting( ech, TRUE );
    if( IS_NPC( ech ) )
    {
      /*
       * if mob is in area, or part of area. 
       */
      if( URANGE( pArea->low_m_vnum, ech->pIndexData->vnum,
                  pArea->hi_m_vnum ) == ech->pIndexData->vnum || ( ech->in_room && ech->in_room->area == pArea ) )
        extract_char( ech, TRUE );
      continue;
    }
    if( ech->in_room && ech->in_room->area == pArea )
      do_recall( ech, "" );
  }
  for( eobj = first_object; eobj; eobj = eobj_next )
  {
    eobj_next = eobj->next;
    /*
     * if obj is in area, or part of area. 
     */
    if( URANGE( pArea->low_o_vnum, eobj->pIndexData->vnum,
                pArea->hi_o_vnum ) == eobj->pIndexData->vnum || ( eobj->in_room && eobj->in_room->area == pArea ) )
      extract_obj( eobj );
  }
  for( icnt = 0; icnt < MAX_KEY_HASH; icnt++ )
  {
    for( rid = room_index_hash[icnt]; rid; rid = rid_next )
    {
      rid_next = rid->next;

      for( exit = rid->first_exit; exit; exit = exit_next )
      {
        exit_next = exit->next;
        if( rid->area == pArea || exit->to_room->area == pArea )
        {
          STRFREE( exit->keyword );
          STRFREE( exit->description );
          UNLINK( exit, rid->first_exit, rid->last_exit, next, prev );
          DISPOSE( exit );
          /*
           * Crash bug fix.  I know it could go from the start several times
           * * But you CAN NOT iterate over a link-list and DELETE from it or
           * * Nasty things can and will happen. --Shaddai
           */
          exit = rid->first_exit;
        }
      }
      if( rid->area != pArea )
        continue;
      STRFREE( rid->name );
      STRFREE( rid->description );
      if( rid->first_person )
      {
        bug( "close_area: room with people #%d", rid->vnum );
        for( ech = rid->first_person; ech; ech = ech_next )
        {
          ech_next = ech->next_in_room;
          if( ech->fighting )
            stop_fighting( ech, TRUE );
          if( IS_NPC( ech ) )
            extract_char( ech, TRUE );
          else
            do_recall( ech, "" );
        }
      }
      if( rid->first_content )
      {
        bug( "close_area: room with contents #%d", rid->vnum );
        for( eobj = rid->first_content; eobj; eobj = eobj_next )
        {
          eobj_next = eobj->next_content;
          extract_obj( eobj );
        }
      }
      for( eed = rid->first_extradesc; eed; eed = eed_next )
      {
        eed_next = eed->next;
        STRFREE( eed->keyword );
        STRFREE( eed->description );
        DISPOSE( eed );
      }
      for( mpact = rid->mpact; mpact; mpact = mpact_next )
      {
        mpact_next = mpact->next;
        STRFREE( mpact->buf );
        DISPOSE( mpact );
      }
      for( mprog = rid->mudprogs; mprog; mprog = mprog_next )
      {
        mprog_next = mprog->next;
        STRFREE( mprog->arglist );
        STRFREE( mprog->comlist );
        DISPOSE( mprog );
      }
      if( rid == room_index_hash[icnt] )
        room_index_hash[icnt] = rid->next;
      else
      {
        ROOM_INDEX_DATA *trid;

        for( trid = room_index_hash[icnt]; trid; trid = trid->next )
          if( trid->next == rid )
            break;
        if( !trid )
          bug( "Close_area: rid not in hash list %d", rid->vnum );
        else
          trid->next = rid->next;
      }
      DISPOSE( rid );
    }

    for( mid = mob_index_hash[icnt]; mid; mid = mid_next )
    {
      mid_next = mid->next;

      if( mid->vnum < pArea->low_m_vnum || mid->vnum > pArea->hi_m_vnum )
        continue;

      STRFREE( mid->player_name );
      STRFREE( mid->short_descr );
      STRFREE( mid->long_descr );
      STRFREE( mid->description );
      if( mid->pShop )
      {
        UNLINK( mid->pShop, first_shop, last_shop, next, prev );
        DISPOSE( mid->pShop );
      }
      if( mid->rShop )
      {
        UNLINK( mid->rShop, first_repair, last_repair, next, prev );
        DISPOSE( mid->rShop );
      }
      for( mprog = mid->mudprogs; mprog; mprog = mprog_next )
      {
        mprog_next = mprog->next;
        STRFREE( mprog->arglist );
        STRFREE( mprog->comlist );
        DISPOSE( mprog );
      }
      if( mid == mob_index_hash[icnt] )
        mob_index_hash[icnt] = mid->next;
      else
      {
        MOB_INDEX_DATA *tmid;

        for( tmid = mob_index_hash[icnt]; tmid; tmid = tmid->next )
          if( tmid->next == mid )
            break;
        if( !tmid )
          bug( "Close_area: mid not in hash list %d", mid->vnum );
        else
          tmid->next = mid->next;
      }
      DISPOSE( mid );
    }

    for( oid = obj_index_hash[icnt]; oid; oid = oid_next )
    {
      oid_next = oid->next;

      if( oid->vnum < pArea->low_o_vnum || oid->vnum > pArea->hi_o_vnum )
        continue;

      STRFREE( oid->name );
      STRFREE( oid->short_descr );
      STRFREE( oid->description );
      STRFREE( oid->action_desc );

      for( eed = oid->first_extradesc; eed; eed = eed_next )
      {
        eed_next = eed->next;
        STRFREE( eed->keyword );
        STRFREE( eed->description );
        DISPOSE( eed );
      }
      for( paf = oid->first_affect; paf; paf = paf_next )
      {
        paf_next = paf->next;
        DISPOSE( paf );
      }
      for( mprog = oid->mudprogs; mprog; mprog = mprog_next )
      {
        mprog_next = mprog->next;
        STRFREE( mprog->arglist );
        STRFREE( mprog->comlist );
        DISPOSE( mprog );
      }
      if( oid == obj_index_hash[icnt] )
        obj_index_hash[icnt] = oid->next;
      else
      {
        OBJ_INDEX_DATA *toid;

        for( toid = obj_index_hash[icnt]; toid; toid = toid->next )
          if( toid->next == oid )
            break;
        if( !toid )
          bug( "Close_area: oid not in hash list %d", oid->vnum );
        else
          toid->next = oid->next;
      }
      DISPOSE( oid );
    }
  }
  for( ereset = pArea->first_reset; ereset; ereset = ereset_next )
  {
    ereset_next = ereset->next;
    DISPOSE( ereset );
  }
  DISPOSE( pArea->name );
  DISPOSE( pArea->filename );
  STRFREE( pArea->author );
  UNLINK( pArea, first_build, last_build, next, prev );
  UNLINK( pArea, first_asort, last_asort, next_sort, prev_sort );
  DISPOSE( pArea );
}

void do_undelete( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  char *name;

  set_char_color( AT_RED, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Undelete what player file?\n\r", ch );
    return;
  }

  for( victim = first_char; victim; victim = victim->next )
    if( !IS_NPC( victim ) && !str_cmp( victim->name, arg ) )
      break;

  name = capitalize( arg );
  sprintf( buf, "%s%c/%s", BACKUP_DIR, tolower( arg[0] ), name );
  sprintf( buf2, "%s%c/%s", PLAYER_DIR, tolower( arg[0] ), name );
  if( !rename( buf, buf2 ) )
  {
    set_char_color( AT_RED, ch );
    ch_printf( ch, "Player %s undestroyed.  Pfile readded to player directory.\n\r", name );
  }
  else if( errno == ENOENT )
  {
    set_char_color( AT_PLAIN, ch );
    send_to_char( "Player does not exist.\n\r", ch );
  }
  else
  {
    set_char_color( AT_WHITE, ch );
    ch_printf( ch, "Unknown error #%d - %s.  Report.\n\r", errno, strerror( errno ) );
    sprintf( buf, "%s undeleteing %s", ch->name, arg );
    perror( buf );
  }
  return;
}


void do_destroy( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  OBJ_DATA *o;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  char *name;

  set_char_color( AT_RED, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Destroy what player file?\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "normed" ) )
  {
    set_char_color( AT_RED, ch );
    ch_printf( ch, "Player %s destroyed.  Pfile saved in backup directory.\n\r", capitalize( arg ) );
    send_to_char( "Player's area data destroyed.  Area saved as backup.\n\r", ch );
    set_char_color( AT_PLAIN, ch );
    return;
  }

  for( victim = first_char; victim; victim = victim->next )
    if( !IS_NPC( victim ) && !str_cmp( victim->name, arg ) )
      break;

  if( victim )
  {
    if( victim->rank > 0 )
    {
      ch_printf( ch, "Remove their kaio rank first.\n\r" );
      return;
    }
  }

  if( !victim )
  {
    DESCRIPTOR_DATA *d;

    /*
     * Make sure they aren't halfway logged in. 
     */
    for( d = first_descriptor; d; d = d->next )
      if( ( victim = d->character ) && !IS_NPC( victim ) && !str_cmp( victim->name, arg ) )
        break;
    if( d )
      close_socket( d, TRUE );
  }
  else
  {
    int x, y;

    add_hiscore( "pkill", victim->name, 0 );
    add_hiscore( "sparwins", victim->name, 0 );
    add_hiscore( "sparloss", victim->name, 0 );
    add_hiscore( "mkills", victim->name, 0 );
    add_hiscore( "deaths", victim->name, 0 );
    add_hiscore_ld( "powerlevel", victim->name, 0 );
    if( is_saiyan( ch ) )
      add_hiscore_ld( "plsaiyan", victim->name, 0 );
    if( is_human( ch ) )
      add_hiscore_ld( "plhuman", victim->name, 0 );
    if( is_hb( ch ) )
      add_hiscore_ld( "plhalfbreed", victim->name, 0 );
    if( is_namek( ch ) )
      add_hiscore_ld( "plnamek", victim->name, 0 );
    if( is_android( ch ) )
      add_hiscore_ld( "plandroid", victim->name, 0 );
    if( IS_ICER( ch ) )
      add_hiscore_ld( "plicer", victim->name, 0 );
    if( is_bio( ch ) )
      add_hiscore_ld( "plbio-android", victim->name, 0 );
    if( IS_KAIO( ch ) )
      add_hiscore_ld( "plkaio", victim->name, 0 );
    if( IS_DEMON( ch ) )
      add_hiscore_ld( "pldemon", victim->name, 0 );
    if( is_genie( ch ) )
      add_hiscore_ld( "plgenie", victim->name, 0 );
    if( is_saibaman( ch ) )
      add_hiscore_ld( "plsaibaman", victim->name, 0 );
    if( is_fierian( ch ) )
      add_hiscore_ld( "plfierian", victim->name, 0 );
    if( is_hylian( ch ) )
      add_hiscore_ld( "plhylian", victim->name, 0 );
    if( is_reploid( ch ) )
      add_hiscore_ld( "plreploid", victim->name, 0 );

    add_hiscore( "played", victim->name, 0 );
    add_hiscore( "zeni", victim->name, 0 );
    add_hiscore( "bounty", victim->name, 0 );

    while( ( o = carrying_noquit( victim ) ) != NULL )
    {
      obj_from_char( o );
      obj_to_room( o, victim->in_room );
      ch_printf( ch, "&w%s drops %s&w.\n\r", victim->name, o->short_descr );
    }

    quitting_char = victim;
    if( victim->pcdata->clan )
      remove_member( victim );
    save_char_obj( victim );
    saving_char = NULL;
    extract_char( victim, TRUE );
    for( x = 0; x < MAX_WEAR; x++ )
      for( y = 0; y < MAX_LAYERS; y++ )
        save_equipment[x][y] = NULL;
  }

  name = capitalize( arg );
  sprintf( buf, "%s%c/%s", PLAYER_DIR, tolower( arg[0] ), name );
  sprintf( buf2, "%s%c/%s", BACKUP_DIR, tolower( arg[0] ), name );
  if( !rename( buf, buf2 ) )
  {
    AREA_DATA *pArea;

    set_char_color( AT_RED, ch );
    ch_printf( ch, "Player %s destroyed.  Pfile saved in backup directory.\n\r", name );
    remove_from_auth( name );
    sprintf( buf, "%s%s", HOUSE_DIR, name );
    if( !remove( buf ) )
      send_to_char( "Player's housing data destroyed.\n\r", ch );
    else if( errno != ENOENT )
    {
      ch_printf( ch, "Unknown error #%d - %s (housing data)." "  Report to Coder.\n\r", errno, strerror( errno ) );
      sprintf( buf2, "%s destroying %s", ch->name, buf );
      perror( buf2 );
    }

    sprintf( buf, "%s%s", GOD_DIR, name );
    if( !remove( buf ) )
      send_to_char( "Player's immortal data destroyed.\n\r", ch );
    else if( errno != ENOENT )
    {
      ch_printf( ch, "Unknown error #%d - %s (immortal data).  Report to Goku.\n\r", errno, strerror( errno ) );
      sprintf( buf2, "%s destroying %s", ch->name, buf );
      perror( buf2 );
    }

    sprintf( buf2, "%s.are", name );
    for( pArea = first_build; pArea; pArea = pArea->next )
      if( !str_cmp( pArea->filename, buf2 ) )
      {
        sprintf( buf, "%s%s", BUILD_DIR, buf2 );
        if( IS_SET( pArea->status, AREA_LOADED ) )
          fold_area( pArea, buf, FALSE );
        close_area( pArea );
        sprintf( buf2, "%s.bak", buf );
        set_char_color( AT_RED, ch ); /* Log message changes colors */
        if( !rename( buf, buf2 ) )
          send_to_char( "Player's area data destroyed.  Area saved as backup.\n\r", ch );
        else if( errno != ENOENT )
        {
          ch_printf( ch, "Unknown error #%d - %s (area data).  Report to Thoric.\n\r", errno, strerror( errno ) );
          sprintf( buf2, "%s destroying %s", ch->name, buf );
          perror( buf2 );
        }
        break;
      }
  }
  else if( errno == ENOENT )
  {
    set_char_color( AT_PLAIN, ch );
    send_to_char( "Player does not exist.\n\r", ch );
  }
  else
  {
    set_char_color( AT_WHITE, ch );
    ch_printf( ch, "Unknown error #%d - %s.  Report to Thoric.\n\r", errno, strerror( errno ) );
    sprintf( buf, "%s destroying %s", ch->name, arg );
    perror( buf );
  }
  return;
}

extern ROOM_INDEX_DATA *room_index_hash[MAX_KEY_HASH];  /* db.c */

/* Super-AT command:
FOR ALL <action>
FOR MORTALS <action>
FOR GODS <action>
FOR MOBS <action>
FOR EVERYWHERE <action>

Executes action several times, either on ALL players (not including yourself),
MORTALS (including trusted characters), GODS (characters with level higher than
L_HERO), MOBS (Not recommended) or every room (not recommended either!)

If you insert a # in the action, it will be replaced by the name of the target.

If # is a part of the action, the action will be executed for every target
in game. If there is no #, the action will be executed for every room containg
at least one target, but only once per room. # cannot be used with FOR EVERY-
WHERE. # can be anywhere in the action.

Example:

FOR ALL SMILE -> you will only smile once in a room with 2 players.
FOR ALL TWIDDLE # -> In a room with A and B, you will twiddle A then B.

Destroying the characters this command acts upon MAY cause it to fail. Try to
avoid something like FOR MOBS PURGE (although it actually works at my MUD).

FOR MOBS TRANS 3054 (transfer ALL the mobs to Midgaard temple) does NOT work
though :)

The command works by transporting the character to each of the rooms with
target in them. Private rooms are not violated.

*/

/* Expand the name of a character into a string that identifies THAT
   character within a room. E.g. the second 'guard' -> 2. guard
*/
const char *name_expand( CHAR_DATA * ch )
{
  int count = 1;
  CHAR_DATA *rch;
  char name[MAX_INPUT_LENGTH];  /*  HOPEFULLY no mob has a name longer than THAT */

  static char outbuf[MAX_INPUT_LENGTH];

  if( !IS_NPC( ch ) )
    return ch->name;

  one_argument( ch->name, name ); /* copy the first word into name */

  if( !name[0] )  /* weird mob .. no keywords */
  {
    strcpy( outbuf, "" ); /* Do not return NULL, just an empty buffer */
    return outbuf;
  }

  /*
   * ->people changed to ->first_person -- TRI 
   */
  for( rch = ch->in_room->first_person; rch && ( rch != ch ); rch = rch->next_in_room )
    if( is_name( name, rch->name ) )
      count++;


  sprintf( outbuf, "%d.%s", count, name );
  return outbuf;
}

void do_for( CHAR_DATA * ch, char *argument )
{
  char range[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  bool fGods = FALSE, fMortals = FALSE, fMobs = FALSE, fEverywhere = FALSE, found;
  ROOM_INDEX_DATA *room, *old_room;
  CHAR_DATA *p, *p_prev;  /* p_next to p_prev -- TRI */
  int i;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, range );
  if( !range[0] || !argument[0] ) /* invalid usage? */
  {
    do_help( ch, "for" );
    return;
  }

  if( !str_prefix( "quit", argument ) )
  {
    send_to_char( "Are you trying to crash the MUD or something?\n\r", ch );
    return;
  }


  if( !str_cmp( range, "all" ) )
  {
    fMortals = TRUE;
    fGods = TRUE;
  }
  else if( !str_cmp( range, "gods" ) )
    fGods = TRUE;
  else if( !str_cmp( range, "mortals" ) )
    fMortals = TRUE;
  else if( !str_cmp( range, "mobs" ) )
    fMobs = TRUE;
  else if( !str_cmp( range, "everywhere" ) )
    fEverywhere = TRUE;
  else
    do_help( ch, "for" ); /* show syntax */

  /*
   * do not allow # to make it easier 
   */
  if( fEverywhere && strchr( argument, '#' ) )
  {
    send_to_char( "Cannot use FOR EVERYWHERE with the # thingie.\n\r", ch );
    return;
  }

  set_char_color( AT_PLAIN, ch );
  if( strchr( argument, '#' ) ) /* replace # ? */
  {
    /*
     * char_list - last_char, p_next - gch_prev -- TRI 
     */
    for( p = last_char; p; p = p_prev )
    {
      p_prev = p->prev; /* TRI */
      /*
       * p_next = p->next; 
       *//*
       * In case someone DOES try to AT MOBS SLAY # 
       */
      found = FALSE;

      if( !( p->in_room ) || room_is_private( p->in_room ) || ( p == ch ) )
        continue;

      if( IS_NPC( p ) && fMobs )
        found = TRUE;
      else if( !IS_NPC( p ) && p->level >= LEVEL_IMMORTAL && fGods )
        found = TRUE;
      else if( !IS_NPC( p ) && p->level < LEVEL_IMMORTAL && fMortals )
        found = TRUE;

      /*
       * It looks ugly to me.. but it works :) 
       */
      if( found ) /* p is 'appropriate' */
      {
        char *pSource = argument; /* head of buffer to be parsed */
        char *pDest = buf;  /* parse into this */

        while( *pSource )
        {
          if( *pSource == '#' ) /* Replace # with name of target */
          {
            const char *namebuf = name_expand( p );

            if( namebuf ) /* in case there is no mob name ?? */
              while( *namebuf ) /* copy name over */
                *( pDest++ ) = *( namebuf++ );

            pSource++;
          }
          else
            *( pDest++ ) = *( pSource++ );
        } /* while */
        *pDest = '\0';  /* Terminate */

        /*
         * Execute 
         */
        old_room = ch->in_room;
        char_from_room( ch );
        char_to_room( ch, p->in_room );
        interpret( ch, buf );
        char_from_room( ch );
        char_to_room( ch, old_room );

      } /* if found */
    } /* for every char */
  }
  else  /* just for every room with the appropriate people in it */
  {
    for( i = 0; i < MAX_KEY_HASH; i++ ) /* run through all the buckets */
      for( room = room_index_hash[i]; room; room = room->next )
      {
        found = FALSE;

        /*
         * Anyone in here at all? 
         */
        if( fEverywhere ) /* Everywhere executes always */
          found = TRUE;
        else if( !room->first_person )  /* Skip it if room is empty */
          continue;
        /*
         * ->people changed to first_person -- TRI 
         */

        /*
         * Check if there is anyone here of the requried type 
         */
        /*
         * Stop as soon as a match is found or there are no more ppl in room 
         */
        /*
         * ->people to ->first_person -- TRI 
         */
        for( p = room->first_person; p && !found; p = p->next_in_room )
        {

          if( p == ch ) /* do not execute on oneself */
            continue;

          if( IS_NPC( p ) && fMobs )
            found = TRUE;
          else if( !IS_NPC( p ) && ( p->level >= LEVEL_IMMORTAL ) && fGods )
            found = TRUE;
          else if( !IS_NPC( p ) && ( p->level <= LEVEL_IMMORTAL ) && fMortals )
            found = TRUE;
        } /* for everyone inside the room */

        if( found && !room_is_private( room ) ) /* Any of the required type here AND room not private? */
        {
          /*
           * This may be ineffective. Consider moving character out of old_room
           * once at beginning of command then moving back at the end.
           * This however, is more safe?
           */

          old_room = ch->in_room;
          char_from_room( ch );
          char_to_room( ch, room );
          interpret( ch, argument );
          char_from_room( ch );
          char_to_room( ch, old_room );
        } /* if found */
      } /* for every room in a bucket */
  } /* if strchr */
} /* do_for */

void save_sysdata args( ( SYSTEM_DATA sys ) );

void do_cset( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_STRING_LENGTH];
  sh_int level;

  set_pager_color( AT_PLAIN, ch );

  if( argument[0] == '\0' )
  {
    pager_printf_color( ch, "\n\r&WMud_name: %s", sysdata.mud_name );
    pager_printf_color( ch,
                        "\n\r&WMail:\n\r  &wRead all mail: &W%d  &wRead mail for free: &W%d  &wWrite mail for free: &W%d\n\r",
                        sysdata.read_all_mail, sysdata.read_mail_free, sysdata.write_mail_free );
    pager_printf_color( ch, "  &wTake all mail: &W%d  &wIMC mail board vnum: &W%d\n\r", sysdata.take_others_mail,
                        sysdata.imc_mail_vnum );
    pager_printf_color( ch, "\n\r&wName authorization: &W%s\n\r", sysdata.WAIT_FOR_AUTH ? "On" : "Off" );
    pager_printf_color( ch, "\n\r&wPfile autocleanup status: &W%s  &wDays before purging newbies: &W%d\n\r",
                        sysdata.CLEANPFILES ? "On" : "Off", sysdata.newbie_purge );
    pager_printf_color( ch, "&wDays before purging regular players: &W%d\n\r", sysdata.regular_purge );
    pager_printf_color( ch, "&WChannels:\n\r  &wMuse: &W%d   &wThink: &W%d   &wLog: &W%d   &wBuild: &W%d\n\r",
                        sysdata.muse_level, sysdata.think_level, sysdata.log_level, sysdata.build_level );
    pager_printf_color( ch, "&WBuilding:\n\r  &wPrototype modification: &W%d  &wPlayer msetting: &W%d\n\r",
                        sysdata.level_modify_proto, sysdata.level_mset_player );
    pager_printf_color( ch, "&WGuilds:\n\r  &wOverseer: &W%s   &wAdvisor: &W%s\n\r",
                        sysdata.guild_overseer, sysdata.guild_advisor );
    pager_printf_color( ch, "&WBan Data:\n\r  &wBan Site Level: &W%d   &wBan Class Level: &W%d   ",
                        sysdata.ban_site_level, sysdata.ban_class_level );
    pager_printf_color( ch, "&wBan Race Level: &W%d\n\r", sysdata.ban_race_level );
    pager_printf_color( ch, "&WDefenses:\n\r  &wDodge_mod: &W%d    &wParry_mod: &W%d    &wTumble_mod: &W%d\n\r",
                        sysdata.dodge_mod, sysdata.parry_mod, sysdata.tumble_mod );
    pager_printf_color( ch, "&WOther:\n\r  &wForce on players:             &W%-2d     ", sysdata.level_forcepc );
    pager_printf_color( ch, "&wPrivate room override:         &W%-2d\n\r", sysdata.level_override_private );
    pager_printf_color( ch, "  &wPenalty to bash plr vs. plr:  &W%-7d", sysdata.bash_plr_vs_plr );
    pager_printf_color( ch, "&wPenalty to non-tank bash:      &W%-3d\n\r", sysdata.bash_nontank );
    pager_printf_color( ch, "  &wPenalty to gouge plr vs. plr: &W%-7d", sysdata.gouge_plr_vs_plr );
    pager_printf_color( ch, "&wPenalty to non-tank gouge:     &W%-3d\n\r", sysdata.gouge_nontank );
    pager_printf_color( ch, "  &wPenalty regular stun chance:  &W%-7d", sysdata.stun_regular );
    pager_printf_color( ch, "&wPenalty to stun plr vs. plr:   &W%-3d\n\r", sysdata.stun_plr_vs_plr );
    pager_printf_color( ch, "  &wPercent damage plr vs. plr:   &W%-7d", sysdata.dam_plr_vs_plr );
    pager_printf_color( ch, "&wPercent damage plr vs. mob:    &W%-3d \n\r", sysdata.dam_plr_vs_mob );
    pager_printf_color( ch, "  &wPercent damage mob vs. plr:   &W%-7d", sysdata.dam_mob_vs_plr );
    pager_printf_color( ch, "&wPercent damage mob vs. mob:    &W%-3d\n\r", sysdata.dam_mob_vs_mob );
    pager_printf_color( ch, "  &wGet object without take flag: &W%-7d", sysdata.level_getobjnotake );
    pager_printf_color( ch, "&wAutosave frequency (minutes):  &W%d\n\r", sysdata.save_frequency );
    pager_printf_color( ch, "  &wMax level difference bestow:  &W%-7d", sysdata.bestow_dif );
    pager_printf_color( ch, "&wChecking Imm_host is:          &W%s\n\r", ( sysdata.check_imm_host ) ? "ON" : "off" );
    pager_printf_color( ch, "  &wMorph Optimization is:        &W%-7s", ( sysdata.morph_opt ) ? "ON" : "off" );
    pager_printf_color( ch, "&wSaving Pets is:                &W%s\n\r", ( sysdata.save_pets ) ? "ON" : "off" );
    pager_printf_color( ch, "  &wPkill looting is:             &W%-7s", ( sysdata.pk_loot ) ? "ON" : "off" );
    pager_printf_color( ch, "&wWait For Auth is:              &W%s\n\r", ( sysdata.WAIT_FOR_AUTH ) ? "ON" : "off" );
    pager_printf_color( ch, "  &wSave flags: &W%s\n\r", flag_string( sysdata.save_flags, save_flag ) );
    pager_printf_color( ch, "  &wAHelp:                        &W%-7s", ( sysdata.ahelp ) ? "ON" : "off" );
    pager_printf_color( ch, "&wWait For Auth is:              &W%s\n\r", ( sysdata.WAIT_FOR_AUTH ) ? "ON" : "off" );

    pager_printf_color( ch, "  &wMax Player Check is:          &W%-7s\n\r", ( sysdata.check_plimit ) ? "ON" : "off" );
    pager_printf_color( ch, "  &wMax Online Players:           &W%-7d", sysdata.plimit );
    pager_printf_color( ch, "&wNo Max Player Level:           &W%d\n\r", sysdata.level_noplimit );
    pager_printf_color( ch, "  &wHiscore Tables:               &W%-7s", ( sysdata.stall_hiscores ) ? "ON" : "off" );
    pager_printf_color( ch, "&wKai Restore Timer:            &W%d\n\r", sysdata.kaiRestoreTimer );
//    pager_printf_color(ch, "  &wNon-proto invoke level        &W%-7d", sysdata.level_invoke_proto);
    return;
  }

  argument = one_argument( argument, arg );
  smash_tilde( argument );

  if( !str_cmp( arg, "help" ) )
  {
    do_help( ch, "controls" );
    return;
  }

  if( !str_cmp( arg, "auth" ) )
  {

    sysdata.WAIT_FOR_AUTH = !sysdata.WAIT_FOR_AUTH;

    if( sysdata.WAIT_FOR_AUTH )
      send_to_char( "Name authorization system enabled.\n\r", ch );
    else
      send_to_char( "Name authorization system disabled.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "hiscore" ) )
  {

    sysdata.CLEANPFILES = !sysdata.CLEANPFILES;

    if( sysdata.CLEANPFILES )
      send_to_char( "Hiscore tables enabled.\n\r", ch );
    else
      send_to_char( "Hiscore tables disabled.\n\r", ch );
    return;
  }
  if( !str_cmp( arg, "pfiles" ) )
  {

    sysdata.CLEANPFILES = !sysdata.CLEANPFILES;

    if( sysdata.CLEANPFILES )
      send_to_char( "Pfile autocleanup enabled.\n\r", ch );
    else
      send_to_char( "Pfile autocleanup disabled.\n\r", ch );
    return;
  }
  if( !str_cmp( arg, "ahelp" ) )
  {

    sysdata.ahelp = !sysdata.ahelp;

    if( sysdata.ahelp )
      send_to_char( "AHelp enabled.\n\r", ch );
    else
      send_to_char( "AHelp disabled.\n\r", ch );
    return;
  }
  if( !str_cmp( arg, "maxplayercheck" ) )
  {

    sysdata.check_plimit = !sysdata.check_plimit;

    if( sysdata.check_plimit )
      send_to_char( "Max online player check enabled.\n\r", ch );
    else
      send_to_char( "Max online player check disabled.\n\r", ch );
    return;
  }
  if( !str_cmp( arg, "waitforauth" ) )
  {

    sysdata.WAIT_FOR_AUTH = !sysdata.WAIT_FOR_AUTH;

    if( sysdata.WAIT_FOR_AUTH )
      send_to_char( "Wait for auth enabled.\n\r", ch );
    else
      send_to_char( "Wait for auth disabled.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "save" ) )
  {
    save_sysdata( sysdata );
    send_to_char( "Cset functions saved.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "mudname" ) )
  {
    if( sysdata.mud_name )
      DISPOSE( sysdata.mud_name );
    sysdata.mud_name = str_dup( argument );
    send_to_char( "Name set.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "saveflag" ) )
  {
    int x = get_saveflag( argument );

    if( x == -1 )
      send_to_char( "Not a save flag.\n\r", ch );
    else
    {
      TOGGLE_BIT( sysdata.save_flags, 1 << x );
      send_to_char( "Ok.\n\r", ch );
    }
    return;
  }

  if( !str_prefix( arg, "guild_overseer" ) )
  {
    STRFREE( sysdata.guild_overseer );
    sysdata.guild_overseer = STRALLOC( argument );
    send_to_char( "Ok.\n\r", ch );
    return;
  }
  if( !str_prefix( arg, "guild_advisor" ) )
  {
    STRFREE( sysdata.guild_advisor );
    sysdata.guild_advisor = STRALLOC( argument );
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  level = ( sh_int ) atoi( argument );

  if( !str_cmp( arg, "plimit" ) )
  {
    if( level < 1 )
    {
      send_to_char( "You must allow at least 1 player online.\n\r", ch );
      return;
    }

    sysdata.plimit = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "noplimit" ) )
  {
    sysdata.level_noplimit = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_prefix( arg, "savefrequency" ) )
  {
    sysdata.save_frequency = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "newbie_purge" ) )
  {
    if( level < 1 )
    {
      send_to_char( "You must specify a period of at least 1 day.\n\r", ch );
      return;
    }

    sysdata.newbie_purge = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "regular_purge" ) )
  {
    if( level < 1 )
    {
      send_to_char( "You must specify a period of at least 1 day.\n\r", ch );
      return;
    }

    sysdata.regular_purge = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_prefix( arg, "checkimmhost" ) )
  {
    if( level != 0 && level != 1 )
    {
      send_to_char( "Use 1 to turn it on, 0 to turn in off.\n\r", ch );
      return;
    }
    sysdata.check_imm_host = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "bash_pvp" ) )
  {
    sysdata.bash_plr_vs_plr = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "bash_nontank" ) )
  {
    sysdata.bash_nontank = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "gouge_pvp" ) )
  {
    sysdata.gouge_plr_vs_plr = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "gouge_nontank" ) )
  {
    sysdata.gouge_nontank = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "dodge_mod" ) )
  {
    sysdata.dodge_mod = level > 0 ? level : 1;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "parry_mod" ) )
  {
    sysdata.parry_mod = level > 0 ? level : 1;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "tumble_mod" ) )
  {
    sysdata.tumble_mod = level > 0 ? level : 1;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "stun" ) )
  {
    sysdata.stun_regular = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "stun_pvp" ) )
  {
    sysdata.stun_plr_vs_plr = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "dam_pvp" ) )
  {
    sysdata.dam_plr_vs_plr = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "get_notake" ) )
  {
    sysdata.level_getobjnotake = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "dam_pvm" ) )
  {
    sysdata.dam_plr_vs_mob = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "dam_mvp" ) )
  {
    sysdata.dam_mob_vs_plr = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "dam_mvm" ) )
  {
    sysdata.dam_mob_vs_mob = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "imc_vnum" ) || !str_cmp( arg, "imc_mail_vnum" ) )
  {
    sysdata.imc_mail_vnum = level;
    send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "ident_retries" ) || !str_cmp( arg, "ident" ) )
  {
    sysdata.ident_retries = level;
    if( level > 20 )
      send_to_char( "Caution:  This setting may cause the game to lag.\n\r", ch );
    else if( level <= 0 )
      send_to_char( "Ident lookups turned off.\n\r", ch );
    else
      send_to_char( "Ok.\n\r", ch );
    return;
  }

  if( level < 0 || level > MAX_LEVEL + 1 )
  {
    send_to_char( "Invalid value for new control.\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "read_all" ) )
    sysdata.read_all_mail = level;
  else if( !str_cmp( arg, "read_free" ) )
    sysdata.read_mail_free = level;
  else if( !str_cmp( arg, "write_free" ) )
    sysdata.write_mail_free = level;
  else if( !str_cmp( arg, "take_all" ) )
    sysdata.take_others_mail = level;
  else if( !str_cmp( arg, "muse" ) )
    sysdata.muse_level = level;
  else if( !str_cmp( arg, "think" ) )
    sysdata.think_level = level;
  else if( !str_cmp( arg, "log" ) )
    sysdata.log_level = level;
  else if( !str_cmp( arg, "build" ) )
    sysdata.build_level = level;
  else if( !str_cmp( arg, "proto_modify" ) )
    sysdata.level_modify_proto = level;
  else if( !str_cmp( arg, "invoke_proto" ) )
    sysdata.level_invoke_proto = level;
  else if( !str_cmp( arg, "kai_timer" ) )
    sysdata.kaiRestoreTimer = level;
  else if( !str_cmp( arg, "override_private" ) )
    sysdata.level_override_private = level;
  else if( !str_cmp( arg, "bestow_dif" ) )
    sysdata.bestow_dif = level > 0 ? level : 1;
  else if( !str_cmp( arg, "forcepc" ) )
    sysdata.level_forcepc = level;
  else if( !str_cmp( arg, "ban_site_level" ) )
    sysdata.ban_site_level = level;
  else if( !str_cmp( arg, "ban_race_level" ) )
    sysdata.ban_race_level = level;
  else if( !str_cmp( arg, "ban_class_level" ) )
    sysdata.ban_class_level = level;
  else if( !str_cmp( arg, "petsave" ) )
  {
    if( level )
      sysdata.save_pets = TRUE;
    else
      sysdata.save_pets = FALSE;
  }
  else if( !str_cmp( arg, "pk_loot" ) )
  {
    if( level )
    {
      send_to_char( "Pkill looting is enabled.\n\r", ch );
      sysdata.pk_loot = TRUE;
    }
    else
    {
      send_to_char( "Pkill looting is disabled.  (use cset pkloot 1 to enable)\n\r", ch );
      sysdata.pk_loot = FALSE;
    }
  }
  else if( !str_cmp( arg, "morph_opt" ) )
  {
    if( level )
      sysdata.morph_opt = TRUE;
    else
      sysdata.morph_opt = FALSE;
  }
  else if( !str_cmp( arg, "mset_player" ) )
    sysdata.level_mset_player = level;
  else
  {
    send_to_char( "Invalid argument.\n\r", ch );
    return;
  }
  send_to_char( "Ok.\n\r", ch );
  return;
}

void get_reboot_string( void )
{
  sprintf( reboot_time, "%s", asctime( new_boot_time ) );
}

void do_orange( CHAR_DATA * ch, char *argument )
{
  send_to_char_color( "&YFunction under construction.\n\r", ch );
  return;
}

void do_mrange( CHAR_DATA * ch, char *argument )
{
  send_to_char_color( "&YFunction under construction.\n\r", ch );
  return;
}


/* Admin command to automatically dock someones PL. -- By Chili
 * Syntax: pldock <player> <percentage> <reason>
 */
void do_pldock (CHAR_DATA *ch, char *argument) {
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  char tbuf[MAX_STRING_LENGTH];
  char *tbp;
  char *p;
  sh_int trust_level;
  float max_perc = 0.0;
  float max_perc_c;
  float perc_dock;

  if((trust_level = get_trust(ch)) < (MAX_LEVEL-9)) {
    send_to_char("You don't look that special to me.\r\n", ch);
    return;
  }
  trust_level = trust_level - 55;
  /* First calculate the most percentage the guy can take off */
  max_perc = (float) trust_level / 9.0;

  for(max_perc_c = 0.15; max_perc_c < 1.0; max_perc_c += .10)
    if(max_perc < max_perc_c) {
      max_perc = max_perc_c;
      break;
    }

  /* Now find this guy they wanna fuck with */

  argument = one_argument(argument, arg);
  
  if(!*arg) {
    send_to_char("Did you take your meds today?\r\n", ch);
    return;
  }

  if(!(victim = get_char_world(ch, arg)) || IS_NPC(victim)) {
    send_to_char("They aren't here.\r\n", ch);
    return;
  }

  if(IS_IMMORTAL(victim)) {
    send_to_char("No reason to dock an immortals powerlevel.\r\n", ch);
    return;
  }

  argument = one_argument(argument, arg);

  if(!(p = strchr(arg, '%')) || *p != '%') {
    send_to_char("That don't look like a percentage to me.\r\n", ch);
    return;
  }

  *p = '\0';
  perc_dock = atof(arg) * .01;

  if(perc_dock < 0.01) {
    send_to_char("Is there really a point to use this command in the first place?\r\n", ch);
    return;
  }

  if(perc_dock > max_perc) {
    send_to_char("You can't dock players powerlevel that much.\r\n", ch);
    return;
  }

  if(!argument || !*argument) {
    send_to_char("You must specify a reason to dock a players powerlevel.\r\n", ch);
    return;
  }

  tbp = tbuf;
  sbuf_format(tbuf, &tbp, "%s has been administratively pldocked by %s.\r\n", victim->name, ch->name);
  sbuf_format(tbuf, &tbp, "Percentage PL Docked: %s\%\r\n", arg);
  sbuf_format(tbuf, &tbp, "Previous PowerLevel: %s\r\n", num_punct_ld(victim->exp));
  victim->pl = victim->exp = victim->exp - (victim->exp * (long double) perc_dock);

  ch_printf(ch, "You administratively dock '%s' powerlevel down %s%c.\r\nTheir new powerlevel is now set to %s.\r\nYour reason for docking their powerlevel:\r\n\t%s\r\nYour reason has been posted to the MUD punishment board.", victim->name, arg, '%',  num_punct_ld(victim->exp), argument);

  sbuf_format(tbuf, &tbp, "New Powerlevel: %s\r\n", num_punct_ld(victim->exp));
  sbuf_format(tbuf, &tbp, "Reason:\r\n\t%s\r\n", argument);
  *tbp = '\0';
 
  ch_printf(victim, "Your powerlevel has been administratevly docked %s%c by %s.\r\nYour new powerlevel is now set to %s.",  arg, '%', ch->name, num_punct_ld(victim->exp));

  make_note("Punish", "The Punisher", "all enforcer", victim->name, 30,  tbuf);  
}


void do_hell( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  char tbuf[MAX_INPUT_LENGTH];
  sh_int time;
  bool h_d = FALSE;
  struct tm *tms;
  OBJ_DATA *o;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( !*arg )
  {
    send_to_char( "Hell who, and for how long?\n\r", ch );
    return;
  }
  if( !( victim = get_char_world( ch, arg ) ) || IS_NPC( victim ) )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_IMMORTAL( victim ) )
  {
    send_to_char( "There is no point in helling an immortal.\n\r", ch );
    return;
  }
  if( victim->pcdata->release_date != 0 )
  {
    ch_printf( ch, "They are already in hell until %24.24s, by %s.\n\r",
               ctime( &victim->pcdata->release_date ), victim->pcdata->helled_by );
    return;
  }

  argument = one_argument( argument, arg );
  if( !*arg || !is_number( arg ) )
  {
    send_to_char( "Hell them for how long?\n\r", ch );
    return;
  }

  time = atoi( arg );
  if( time <= 0 )
  {
    send_to_char( "You cannot hell for zero or negative time.\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg );
  if( !*arg || !str_cmp( arg, "hours" ) )
    h_d = TRUE;
  else if( str_cmp( arg, "days" ) )
  {
    send_to_char( "Is that value in hours or days?\n\r", ch );
    return;
  }
  else if( time > 30 )
  {
    send_to_char( "You may not hell a person for more than 30 days at a time.\n\r", ch );
    return;
  }

  /* Check for reason */

  if(!argument || !*argument) {
    send_to_char("You must supply a reason to send the player to hell.\r\n", ch);
    return;
  }

  sprintf(tbuf, "Character '%s' has been helled for %d %s by %s for the reason of '%s'.", victim->name, time,
      h_d ? "hours" : "days", ch->name, argument);
      

  make_note("Punish", "The Punisher", "all enforcer", victim->name, 30,  tbuf);  

  tms = localtime( &current_time );

  while( ( o = carrying_noquit( victim ) ) != NULL )
  {
    obj_from_char( o );
    obj_to_room( o, victim->in_room );
    ch_printf( victim, "&wYou drop %s&w.\n\r", o->short_descr );
  }

  if( h_d )
    tms->tm_hour += time;
  else
    tms->tm_mday += time;
  victim->pcdata->release_date = mktime( tms );
  victim->pcdata->helled_by = STRALLOC( ch->name );
  ch_printf( ch, "%s will be released from hell at %24.24s.\n\r", victim->name, ctime( &victim->pcdata->release_date ) );
  act( AT_MAGIC, "$n disappears in a cloud of hellish light.", victim, NULL, ch, TO_NOTVICT );
  char_from_room( victim );
  char_to_room( victim, get_room_index( 8 ) );
  act( AT_MAGIC, "$n appears in a could of hellish light.", victim, NULL, ch, TO_NOTVICT );
  do_look( victim, "auto" );
  ch_printf( victim, "The immortals are not pleased with your actions.\n\r"
             "You shall remain in hell for %d %s%s.\n\r", time, ( h_d ? "hour" : "day" ), ( time == 1 ? "" : "s" ) );
  save_char_obj( victim );  /* used to save ch, fixed by Thoric 09/17/96 */
  return;
}

void do_unhell( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *location;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( !*arg )
  {
    send_to_char( "Unhell whom..?\n\r", ch );
    return;
  }
  location = ch->in_room;
  /*
   * ch->in_room = get_room_index(8);
   */
  victim = get_char_world( ch, arg );
  /*
   * ch->in_room = location;          The case of unhell self, etc.
   */
  if( !victim || IS_NPC( victim ) )
  {
    send_to_char( "No such player character present.\n\r", ch );
    return;
  }
  if( victim->in_room->vnum != 8 && victim->in_room->vnum != 1206 && victim->in_room->vnum != 6 )
  {
    send_to_char( "No one like that is in hell.\n\r", ch );
    return;
  }

  if( victim->pcdata->clan )
    location = get_room_index( victim->pcdata->clan->recall );
  else
    location = get_room_index( ROOM_VNUM_TEMPLE );
  if( !location )
    location = ch->in_room;
  MOBtrigger = FALSE;
  act( AT_MAGIC, "$n disappears in a cloud of godly light.", victim, NULL, ch, TO_NOTVICT );
  char_from_room( victim );
  char_to_room( victim, location );
  send_to_char( "The gods have smiled on you and released you from hell early!\n\r", victim );
  do_look( victim, "auto" );
  if( victim != ch )
    send_to_char( "They have been released.\n\r", ch );
  if( victim->pcdata->helled_by )
  {
    if( str_cmp( ch->name, victim->pcdata->helled_by ) )
      ch_printf( ch, "(You should probably write a note to %s, explaining the early release.)\n\r",
                 victim->pcdata->helled_by );
    STRFREE( victim->pcdata->helled_by );
    victim->pcdata->helled_by = NULL;
  }

  MOBtrigger = FALSE;
  act( AT_MAGIC, "$n appears in a cloud of godly light.", victim, NULL, ch, TO_NOTVICT );
  victim->pcdata->release_date = 0;
  save_char_obj( victim );
  return;
}

/* Vnum search command by Swordbearer */
void do_vsearch( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  bool found = FALSE;
  OBJ_DATA *obj;
  OBJ_DATA *in_obj;
  int obj_counter = 1;
  int argi;

  set_pager_color( AT_PLAIN, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Syntax:  vsearch <vnum>.\n\r", ch );
    return;
  }

  argi = atoi( arg );
  if( argi < 0 && argi > 20000 )
  {
    send_to_char( "Vnum out of range.\n\r", ch );
    return;
  }
  for( obj = first_object; obj != NULL; obj = obj->next )
  {
    if( !can_see_obj( ch, obj ) || !( argi == obj->pIndexData->vnum ) )
      continue;

    found = TRUE;
    for( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
      ;

    if( in_obj->carried_by != NULL )
      pager_printf( ch, "[%2d] Level %d %s carried by %s.\n\r",
                    obj_counter, obj->level, obj_short( obj ), PERS( in_obj->carried_by, ch ) );
    else
      pager_printf( ch, "[%2d] [%-5d] %s in %s.\n\r", obj_counter,
                    ( ( in_obj->in_room ) ? in_obj->in_room->vnum : 0 ),
                    obj_short( obj ), ( in_obj->in_room == NULL ) ? "somewhere" : in_obj->in_room->name );

    obj_counter++;
  }

  if( !found )
    send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
  return;
}

/*
 * Simple function to let any imm make any player instantly sober.
 * Saw no need for level restrictions on this.
 * Written by Narn, Apr/96
 */
void do_sober( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MAX_INPUT_LENGTH];

  set_char_color( AT_IMMORT, ch );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Not on mobs.\n\r", ch );
    return;
  }

  if( victim->pcdata )
    victim->pcdata->condition[COND_DRUNK] = 0;
  send_to_char( "Ok.\n\r", ch );
  set_char_color( AT_IMMORT, victim );
  send_to_char( "You feel sober again.\n\r", victim );
  return;
}

/*
 * Free a social structure					-Thoric
 */
void free_social( SOCIALTYPE * social )
{
  if( social->name )
    DISPOSE( social->name );
  if( social->char_no_arg )
    DISPOSE( social->char_no_arg );
  if( social->others_no_arg )
    DISPOSE( social->others_no_arg );
  if( social->char_found )
    DISPOSE( social->char_found );
  if( social->others_found )
    DISPOSE( social->others_found );
  if( social->vict_found )
    DISPOSE( social->vict_found );
  if( social->char_auto )
    DISPOSE( social->char_auto );
  if( social->others_auto )
    DISPOSE( social->others_auto );
  DISPOSE( social );
}

/*
 * Remove a social from it's hash index				-Thoric
 */
void unlink_social( SOCIALTYPE * social )
{
  SOCIALTYPE *tmp, *tmp_next;
  int hash;

  if( !social )
  {
    bug( "Unlink_social: NULL social", 0 );
    return;
  }

  if( social->name[0] < 'a' || social->name[0] > 'z' )
    hash = 0;
  else
    hash = ( social->name[0] - 'a' ) + 1;

  if( social == ( tmp = social_index[hash] ) )
  {
    social_index[hash] = tmp->next;
    return;
  }
  for( ; tmp; tmp = tmp_next )
  {
    tmp_next = tmp->next;
    if( social == tmp_next )
    {
      tmp->next = tmp_next->next;
      return;
    }
  }
}

/*
 * Add a social to the social index table			-Thoric
 * Hashed and insert sorted
 */
void add_social( SOCIALTYPE * social )
{
  int hash, x;
  SOCIALTYPE *tmp, *prev;

  if( !social )
  {
    bug( "Add_social: NULL social", 0 );
    return;
  }

  if( !social->name )
  {
    bug( "Add_social: NULL social->name", 0 );
    return;
  }

  if( !social->char_no_arg )
  {
    bug( "Add_social: NULL social->char_no_arg", 0 );
    return;
  }

  /*
   * make sure the name is all lowercase 
   */
  for( x = 0; social->name[x] != '\0'; x++ )
    social->name[x] = LOWER( social->name[x] );

  if( social->name[0] < 'a' || social->name[0] > 'z' )
    hash = 0;
  else
    hash = ( social->name[0] - 'a' ) + 1;

  if( ( prev = tmp = social_index[hash] ) == NULL )
  {
    social->next = social_index[hash];
    social_index[hash] = social;
    return;
  }

  for( ; tmp; tmp = tmp->next )
  {
    if( ( x = strcmp( social->name, tmp->name ) ) == 0 )
    {
      bug( "Add_social: trying to add duplicate name to bucket %d", hash );
      free_social( social );
      return;
    }
    else if( x < 0 )
    {
      if( tmp == social_index[hash] )
      {
        social->next = social_index[hash];
        social_index[hash] = social;
        return;
      }
      prev->next = social;
      social->next = tmp;
      return;
    }
    prev = tmp;
  }

  /*
   * add to end 
   */
  prev->next = social;
  social->next = NULL;
  return;
}

/*
 * Social editor/displayer/save/delete				-Thoric
 */
void do_sedit( CHAR_DATA * ch, char *argument )
{
  SOCIALTYPE *social;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];

  set_char_color( AT_SOCIAL, ch );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if( arg1[0] == '\0' )
  {
    send_to_char( "Syntax: sedit <social> [field]\n\r", ch );
    send_to_char( "Syntax: sedit <social> create\n\r", ch );
    send_to_char( "Syntax: sedit <social> delete\n\r", ch );
    send_to_char( "Syntax: sedit <social> show\n\r", ch );
    send_to_char( "Syntax: sedit <save>\n\r", ch );
    send_to_char( "\n\rField being one of:\n\r", ch );
    send_to_char( "  cnoarg onoarg cfound ofound vfound cauto oauto\n\r", ch );
    return;
  }

  if( !str_cmp( arg1, "save" ) )
  {
    save_socials(  );
    send_to_char( "Saved.\n\r", ch );
    return;
  }

  social = find_social( arg1 );
  if( !str_cmp( arg2, "create" ) )
  {
    if( social )
    {
      send_to_char( "That social already exists!\n\r", ch );
      return;
    }
    CREATE( social, SOCIALTYPE, 1 );
    social->name = str_dup( arg1 );
    sprintf( arg2, "You %s.", arg1 );
    social->char_no_arg = str_dup( arg2 );
    add_social( social );
    send_to_char( "Social added.\n\r", ch );
    return;
  }

  if( !social )
  {
    send_to_char( "Social not found.\n\r", ch );
    return;
  }

  if( arg2[0] == '\0' || !str_cmp( arg2, "show" ) )
  {
    ch_printf( ch, "Social: %s\n\r\n\rCNoArg: %s\n\r", social->name, social->char_no_arg );
    ch_printf( ch, "ONoArg: %s\n\rCFound: %s\n\rOFound: %s\n\r",
               social->others_no_arg ? social->others_no_arg : "(not set)",
               social->char_found ? social->char_found : "(not set)",
               social->others_found ? social->others_found : "(not set)" );
    ch_printf( ch, "VFound: %s\n\rCAuto : %s\n\rOAuto : %s\n\r",
               social->vict_found ? social->vict_found : "(not set)",
               social->char_auto ? social->char_auto : "(not set)",
               social->others_auto ? social->others_auto : "(not set)" );
    return;
  }
  if( !str_cmp( arg2, "delete" ) )
  {
    unlink_social( social );
    free_social( social );
    send_to_char( "Deleted.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "cnoarg" ) )
  {
    if( argument[0] == '\0' || !str_cmp( argument, "clear" ) )
    {
      send_to_char( "You cannot clear this field.  It must have a message.\n\r", ch );
      return;
    }
    if( social->char_no_arg )
      DISPOSE( social->char_no_arg );
    social->char_no_arg = str_dup( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "onoarg" ) )
  {
    if( social->others_no_arg )
      DISPOSE( social->others_no_arg );
    if( argument[0] != '\0' && str_cmp( argument, "clear" ) )
      social->others_no_arg = str_dup( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "cfound" ) )
  {
    if( social->char_found )
      DISPOSE( social->char_found );
    if( argument[0] != '\0' && str_cmp( argument, "clear" ) )
      social->char_found = str_dup( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "ofound" ) )
  {
    if( social->others_found )
      DISPOSE( social->others_found );
    if( argument[0] != '\0' && str_cmp( argument, "clear" ) )
      social->others_found = str_dup( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "vfound" ) )
  {
    if( social->vict_found )
      DISPOSE( social->vict_found );
    if( argument[0] != '\0' && str_cmp( argument, "clear" ) )
      social->vict_found = str_dup( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "cauto" ) )
  {
    if( social->char_auto )
      DISPOSE( social->char_auto );
    if( argument[0] != '\0' && str_cmp( argument, "clear" ) )
      social->char_auto = str_dup( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "oauto" ) )
  {
    if( social->others_auto )
      DISPOSE( social->others_auto );
    if( argument[0] != '\0' && str_cmp( argument, "clear" ) )
      social->others_auto = str_dup( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( get_trust( ch ) > LEVEL_GREATER && !str_cmp( arg2, "name" ) )
  {
    bool relocate;

    one_argument( argument, arg1 );
    if( arg1[0] == '\0' )
    {
      send_to_char( "Cannot clear name field!\n\r", ch );
      return;
    }
    if( arg1[0] != social->name[0] )
    {
      unlink_social( social );
      relocate = TRUE;
    }
    else
      relocate = FALSE;
    if( social->name )
      DISPOSE( social->name );
    social->name = str_dup( arg1 );
    if( relocate )
      add_social( social );
    send_to_char( "Done.\n\r", ch );
    return;
  }

  /*
   * display usage message 
   */
  do_sedit( ch, "" );
}

/*
 * Free a command structure					-Thoric
 */
void free_command( CMDTYPE * command )
{
  if( command->name )
    DISPOSE( command->name );
  DISPOSE( command );
}

/*
 * Remove a command from it's hash index			-Thoric
 */
void unlink_command( CMDTYPE * command )
{
  CMDTYPE *tmp, *tmp_next;
  int hash;

  if( !command )
  {
    bug( "Unlink_command NULL command", 0 );
    return;
  }

  hash = command->name[0] % 126;

  if( command == ( tmp = command_hash[hash] ) )
  {
    command_hash[hash] = tmp->next;
    return;
  }
  for( ; tmp; tmp = tmp_next )
  {
    tmp_next = tmp->next;
    if( command == tmp_next )
    {
      tmp->next = tmp_next->next;
      return;
    }
  }
}

/*
 * Add a command to the command hash table			-Thoric
 */
void add_command( CMDTYPE * command )
{
  int hash, x;
  CMDTYPE *tmp, *prev;

  if( !command )
  {
    bug( "Add_command: NULL command", 0 );
    return;
  }

  if( !command->name )
  {
    bug( "Add_command: NULL command->name", 0 );
    return;
  }

  if( !command->do_fun )
  {
    bug( "Add_command: NULL command->do_fun", 0 );
    return;
  }

  /*
   * make sure the name is all lowercase 
   */
  for( x = 0; command->name[x] != '\0'; x++ )
    command->name[x] = LOWER( command->name[x] );

  hash = command->name[0] % 126;

  if( ( prev = tmp = command_hash[hash] ) == NULL )
  {
    command->next = command_hash[hash];
    command_hash[hash] = command;
    return;
  }

  /*
   * add to the END of the list 
   */
  for( ; tmp; tmp = tmp->next )
    if( !tmp->next )
    {
      tmp->next = command;
      command->next = NULL;
    }
  return;
}

/*
 * Command editor/displayer/save/delete				-Thoric
 * Added support for interpret flags                            -Shaddai
 */
void do_cedit( CHAR_DATA * ch, char *argument )
{
  CMDTYPE *command;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];

  set_char_color( AT_IMMORT, ch );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if( arg1[0] == '\0' )
  {
    send_to_char( "Syntax: cedit save cmdtable\n\r", ch );
    if( get_trust( ch ) > LEVEL_GREATER )
    {
      send_to_char( "Syntax: cedit <command> create [code]\n\r", ch );
      send_to_char( "Syntax: cedit <command> delete\n\r", ch );
      send_to_char( "Syntax: cedit <command> show\n\r", ch );
      send_to_char( "Syntax: cedit <command> raise\n\r", ch );
      send_to_char( "Syntax: cedit <command> lower\n\r", ch );
      send_to_char( "Syntax: cedit <command> list\n\r", ch );
      send_to_char( "Syntax: cedit <command> [field]\n\r", ch );
      send_to_char( "\n\rField being one of:\n\r", ch );
      send_to_char( "  level position log code flags\n\r", ch );
    }
    return;
  }

  if( get_trust( ch ) > LEVEL_GREATER && !str_cmp( arg1, "save" ) && !str_cmp( arg2, "cmdtable" ) )
  {
    save_commands(  );
    send_to_char( "Saved.\n\r", ch );
    return;
  }

  command = find_command( arg1 );
  if( get_trust( ch ) > LEVEL_GREATER && !str_cmp( arg2, "create" ) )
  {
    if( command )
    {
      send_to_char( "That command already exists!\n\r", ch );
      return;
    }
    CREATE( command, CMDTYPE, 1 );
    command->lag_count = 0; /* FB */
    command->name = str_dup( arg1 );
    command->level = get_trust( ch );
    if( *argument )
      one_argument( argument, arg2 );
    else
      sprintf( arg2, "do_%s", arg1 );
    command->do_fun = skill_function( arg2 );
    add_command( command );
    send_to_char( "Command added.\n\r", ch );
    if( command->do_fun == skill_notfound )
      ch_printf( ch, "Code %s not found.  Set to no code.\n\r", arg2 );
    return;
  }

  if( !command )
  {
    send_to_char( "Command not found.\n\r", ch );
    return;
  }
  else if( command->level > get_trust( ch ) )
  {
    send_to_char( "You cannot touch this command.\n\r", ch );
    return;
  }

  if( arg2[0] == '\0' || !str_cmp( arg2, "show" ) )
  {
    ch_printf( ch, "Command:  %s\n\rLevel:    %d\n\rPosition: %d\n\rLog:      %d\n\rCode:     %s\n\rFlags:  %s\n\r",
               command->name, command->level, command->position, command->log,
               skill_name( command->do_fun ), flag_string( command->flags, cmd_flags ) );
    if( command->userec.num_uses )
      send_timer( &command->userec, ch );
    return;
  }

  if( get_trust( ch ) <= LEVEL_GREATER )
  {
    do_cedit( ch, "" );
    return;
  }

  if( !str_cmp( arg2, "raise" ) )
  {
    CMDTYPE *tmp, *tmp_next;
    int hash = command->name[0] % 126;

    if( ( tmp = command_hash[hash] ) == command )
    {
      send_to_char( "That command is already at the top.\n\r", ch );
      return;
    }
    if( tmp->next == command )
    {
      command_hash[hash] = command;
      tmp_next = tmp->next;
      tmp->next = command->next;
      command->next = tmp;
      ch_printf( ch, "Moved %s above %s.\n\r", command->name, command->next->name );
      return;
    }
    for( ; tmp; tmp = tmp->next )
    {
      tmp_next = tmp->next;
      if( tmp_next->next == command )
      {
        tmp->next = command;
        tmp_next->next = command->next;
        command->next = tmp_next;
        ch_printf( ch, "Moved %s above %s.\n\r", command->name, command->next->name );
        return;
      }
    }
    send_to_char( "ERROR -- Not Found!\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "lower" ) )
  {
    CMDTYPE *tmp, *tmp_next;
    int hash = command->name[0] % 126;

    if( command->next == NULL )
    {
      send_to_char( "That command is already at the bottom.\n\r", ch );
      return;
    }
    tmp = command_hash[hash];
    if( tmp == command )
    {
      tmp_next = tmp->next;
      command_hash[hash] = command->next;
      command->next = tmp_next->next;
      tmp_next->next = command;

      ch_printf( ch, "Moved %s below %s.\n\r", command->name, tmp_next->name );
      return;
    }
    for( ; tmp; tmp = tmp->next )
    {
      if( tmp->next == command )
      {
        tmp_next = command->next;
        tmp->next = tmp_next;
        command->next = tmp_next->next;
        tmp_next->next = command;

        ch_printf( ch, "Moved %s below %s.\n\r", command->name, tmp_next->name );
        return;
      }
    }
    send_to_char( "ERROR -- Not Found!\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "list" ) )
  {
    CMDTYPE *tmp;
    int hash = command->name[0] % 126;

    pager_printf( ch, "Priority placement for [%s]:\n\r", command->name );
    for( tmp = command_hash[hash]; tmp; tmp = tmp->next )
    {
      if( tmp == command )
        set_pager_color( AT_GREEN, ch );
      else
        set_pager_color( AT_PLAIN, ch );
      pager_printf( ch, "  %s\n\r", tmp->name );
    }
    return;
  }
  if( !str_cmp( arg2, "delete" ) )
  {
    unlink_command( command );
    free_command( command );
    send_to_char( "Deleted.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "code" ) )
  {
    DO_FUN *fun = skill_function( argument );

    if( fun == skill_notfound )
    {
      send_to_char( "Code not found.\n\r", ch );
      return;
    }
    command->do_fun = fun;
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "level" ) )
  {
    int level = atoi( argument );

    if( ( level < 0 || level > get_trust( ch ) ) )
    {
      send_to_char( "Level out of range.\n\r", ch );
      return;
    }
    command->level = level;
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "log" ) )
  {
    int log = atoi( argument );

    if( log < 0 || log > LOG_COMM )
    {
      send_to_char( "Log out of range.\n\r", ch );
      return;
    }
    command->log = log;
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "position" ) )
  {
    int position = atoi( argument );

    if( position < 0 || position > POS_DRAG )
    {
      send_to_char( "Position out of range.\n\r", ch );
      return;
    }
    command->position = position;
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "flags" ) )
  {
    int flag;
    if( is_number( argument ) )
      flag = atoi( argument );
    else
      flag = get_cmdflag( argument );
    if( flag < 0 || flag >= 32 )
    {
      if( is_number( argument ) )
        ch_printf( ch, "Invalid flag: range is from 0 to 31.\n" );
      else
        ch_printf( ch, "Unknown flag %s.\n", argument );
      return;
    }

    TOGGLE_BIT( command->flags, 1 << flag );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "name" ) )
  {
    bool relocate;

    one_argument( argument, arg1 );
    if( arg1[0] == '\0' )
    {
      send_to_char( "Cannot clear name field!\n\r", ch );
      return;
    }
    if( arg1[0] != command->name[0] )
    {
      unlink_command( command );
      relocate = TRUE;
    }
    else
      relocate = FALSE;
    if( command->name )
      DISPOSE( command->name );
    command->name = str_dup( arg1 );
    if( relocate )
      add_command( command );
    send_to_char( "Done.\n\r", ch );
    return;
  }

  /*
   * display usage message 
   */
  do_cedit( ch, "" );
}

/*
 * Display class information					-Thoric
 */
void do_showclass( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  struct class_type *class;
  int cl, low, hi;
  int i, ct;

  set_pager_color( AT_PLAIN, ch );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  /*
   * if ( arg1[0] == '\0' )
   * {
   * send_to_char( "Syntax: showclass <class> [level range]\n\r", ch );
   * return ;
   * }
   */

  if( arg1[0] == '\0' )
  {
    send_to_char( "\n\rSyntax: showclass  \n\r", ch );
    ct = 0;
    for( i = 0; i < MAX_CLASS; i++ )
    {
      ++ct;
      pager_printf( ch, "%2d> %-16s", i, class_table[i]->who_name );
      if( ct % 4 == 0 )
        send_to_pager( "\n\r", ch );
    }
    send_to_pager( "\n\r", ch );
    return;
  }

  if( is_number( arg1 ) && ( cl = atoi( arg1 ) ) >= 0 && cl < MAX_CLASS )
    class = class_table[cl];
  else
  {
    class = NULL;
    for( cl = 0; cl < MAX_CLASS && class_table[cl]; cl++ )
      if( !str_cmp( class_table[cl]->who_name, arg1 ) )
      {
        class = class_table[cl];
        break;
      }
  }
  if( !class )
  {
    send_to_char( "No such class.\n\r", ch );
    return;
  }
  pager_printf_color( ch, "&wCLASS: &W%s\n\r&wPrime Attribute: &W%-14s  &wWeapon: &W%-5d      &wGuild: &W%-5d\n\r",
                      class->who_name, affect_loc_name( class->attr_prime ), class->weapon, class->guild );
  pager_printf_color( ch, "&wSecond Attribute:  &W%-14s  &wDeficient Attribute:  &W%-14s\n\r",
                      affect_loc_name( class->attr_second ), affect_loc_name( class->attr_deficient ) );
  pager_printf_color( ch, "&wMax Skill Adept: &W%-3d             &wThac0 : &W%-5d     &wThac32: &W%d\n\r",
                      class->skill_adept, class->thac0_00, class->thac0_32 );
  pager_printf_color( ch, "&wHp Min/Hp Max  : &W%-2d/%-2d           &wMana  : &W%-3s      &wExpBase: &W%d\n\r",
                      class->hp_min, class->hp_max, class->fMana ? "yes" : "no ", class->exp_base );
  pager_printf_color( ch, "&wAffected by:  &W%s\n\r", affect_bit_name( &class->affected ) );
  pager_printf_color( ch, "&wResistant to: &W%s\n\r", flag_string( class->resist, ris_flags ) );
  pager_printf_color( ch, "&wSusceptible to: &W%s\n\r", flag_string( class->suscept, ris_flags ) );
  if( arg2[0] != '\0' )
  {
    int x, y, cnt;

    low = UMAX( 0, atoi( arg2 ) );
    hi = URANGE( low, atoi( argument ), MAX_LEVEL );
    for( x = low; x <= hi; x++ )
    {
      set_pager_color( AT_LBLUE, ch );
      pager_printf( ch, "Male: %-30s Female: %s\n\r", title_table[cl][x][0], title_table[cl][x][1] );
      cnt = 0;
      set_pager_color( AT_BLUE, ch );
      for( y = gsn_first_spell; y < gsn_top_sn; y++ )
        if( skill_table[y]->skill_level[cl] == x )
        {
          pager_printf( ch, "  %-7s %-19s%3d     ",
                        skill_tname[skill_table[y]->type], skill_table[y]->name, skill_table[y]->skill_adept[cl] );
          if( ++cnt % 2 == 0 )
            send_to_pager( "\n\r", ch );
        }
      if( cnt % 2 != 0 )
        send_to_pager( "\n\r", ch );
      send_to_pager( "\n\r", ch );
    }
  }
}

/*
 * Create a new class online.				    	-Shaddai
 */
bool create_new_class( int index, char *argument )
{
  int i;

  if( index >= MAX_CLASS || class_table[index] == NULL )
    return FALSE;
  if( class_table[index]->who_name )
    STRFREE( class_table[index]->who_name );
  if( argument[0] != '\0' )
    argument[0] = UPPER( argument[0] );
  class_table[index]->who_name = STRALLOC( argument );
  xCLEAR_BITS( class_table[index]->affected );
  class_table[index]->attr_prime = 0;
  class_table[index]->attr_second = 0;
  class_table[index]->attr_deficient = 0;
  class_table[index]->resist = 0;
  class_table[index]->suscept = 0;
  class_table[index]->weapon = 0;
  class_table[index]->guild = 0;
  class_table[index]->skill_adept = 0;
  class_table[index]->thac0_00 = 0;
  class_table[index]->thac0_32 = 0;
  class_table[index]->hp_min = 0;
  class_table[index]->hp_max = 0;
  class_table[index]->fMana = FALSE;
  class_table[index]->exp_base = 0;
  for( i = 0; i < MAX_LEVEL; i++ )
  {
    title_table[index][i][0] = str_dup( "Not set." );
    title_table[index][i][1] = str_dup( "Not set." );
  }
  return TRUE;
}

/*
 * Edit class information					-Thoric
 */
void do_setclass( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  FILE *fpList;
  char classlist[256];
  struct class_type *class;
  int cl, value, i;

  set_char_color( AT_PLAIN, ch );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if( arg1[0] == '\0' )
  {
    send_to_char( "Syntax: setclass <class> <field> <value>\n\r", ch );
    send_to_char( "Syntax: setclass <class> create\n\r", ch );
    send_to_char( "\n\rField being one of:\n\r", ch );
    send_to_char( "  name prime weapon guild thac0 thac32\n\r", ch );
    send_to_char( "  hpmin hpmax mana expbase mtitle ftitle\n\r", ch );
    send_to_char( "  second, deficient affected resist suscept\n\r", ch );
    send_to_char( "  filename\n\r", ch );
    return;
  }
  if( is_number( arg1 ) && ( cl = atoi( arg1 ) ) >= 0 && cl < MAX_CLASS )
    class = class_table[cl];
  else
  {
    class = NULL;
    for( cl = 0; cl < MAX_CLASS && class_table[cl]; cl++ )
    {
      if( !class_table[cl]->who_name )
        continue;
      if( !str_cmp( class_table[cl]->who_name, arg1 ) )
      {
        class = class_table[cl];
        break;
      }
    }
  }
  if( !str_cmp( arg2, "create" ) && class )
  {
    send_to_char( "That class already exists!\n\r", ch );
    return;
  }

  if( !class && str_cmp( arg2, "create" ) )
  {
    send_to_char( "No such class.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "save" ) )
  {
    write_class_file( cl );
    send_to_char( "Saved.\n\r", ch );
    return;
  }


  if( !str_cmp( arg2, "create" ) )
  {
    if( MAX_PC_CLASS >= MAX_CLASS )
    {
      send_to_char( "You need to up MAX_CLASS in mud and make clean.\n\r", ch );
      return;
    }
    if( ( create_new_class( MAX_PC_CLASS, arg1 ) ) == FALSE )
    {
      send_to_char( "Couldn't create a new class.\n\r", ch );
      return;
    }
    write_class_file( MAX_PC_CLASS );
    MAX_PC_CLASS++;
    sprintf( classlist, "%s%s", CLASS_DIR, CLASS_LIST );

    if( ( fpList = fopen( classlist, "w" ) ) == NULL )
    {
      bug( "Can't open Class list for writing.", 0 );
      return;
    }

    for( i = 0; i < MAX_PC_CLASS; i++ )
      fprintf( fpList, "%s%s.class\n", CLASSDIR, class_table[i]->who_name );

    fprintf( fpList, "$\n" );
    fclose( fpList );
    send_to_char( "Done.\n\r", ch );
    return;
  }

  if( !argument )
  {
    send_to_char( "You must specify an argument.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "name" ) )
  {
    STRFREE( class->who_name );
    class->who_name = STRALLOC( capitalize( argument ) );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "second" ) )
  {
    int x = get_atype( argument );

    if( x < APPLY_STR || ( x > APPLY_CON && x != APPLY_LCK ) )
      send_to_char( "Invalid second attribute!\n\r", ch );
    else
    {
      class->attr_second = x;
      send_to_char( "Done.\n\r", ch );
    }
    return;
  }

  if( !str_cmp( arg2, "affected" ) )
  {
    if( !argument || argument[0] == '\0' )
    {
      send_to_char( "Usage: setclass <class> affected <flag> [flag]...\n\r", ch );
      return;
    }
    while( argument[0] != '\0' )
    {
      argument = one_argument( argument, arg2 );
      value = get_aflag( arg2 );
      if( value < 0 || value > MAX_BITS )
        ch_printf( ch, "Unknown flag: %s\n\r", arg2 );
      else
        xTOGGLE_BIT( class->affected, value );
    }
    send_to_char( "Done.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "resist" ) )
  {
    if( !argument || argument[0] == '\0' )
    {
      send_to_char( "Usage: setclass <class> resist <flag> [flag]...\n\r", ch );
      return;
    }
    while( argument[0] != '\0' )
    {
      argument = one_argument( argument, arg2 );
      value = get_risflag( arg2 );
      if( value < 0 || value > 31 )
        ch_printf( ch, "Unknown flag: %s\n\r", arg2 );
      else
        TOGGLE_BIT( class->resist, 1 << value );
    }
    send_to_char( "Done.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "suscept" ) )
  {
    if( !argument || argument[0] == '\0' )
    {
      send_to_char( "Usage: setclass <class> suscept <flag> [flag]...\n\r", ch );
      return;
    }
    while( argument[0] != '\0' )
    {
      argument = one_argument( argument, arg2 );
      value = get_risflag( arg2 );
      if( value < 0 || value > 31 )
        ch_printf( ch, "Unknown flag: %s\n\r", arg2 );
      else
        TOGGLE_BIT( class->suscept, 1 << value );
    }
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "deficient" ) )
  {
    int x = get_atype( argument );

    if( x < APPLY_STR || ( x > APPLY_CON && x != APPLY_LCK ) )
      send_to_char( "Invalid deficient attribute!\n\r", ch );
    else
    {
      class->attr_deficient = x;
      send_to_char( "Done.\n\r", ch );
    }
    return;
  }
  if( !str_cmp( arg2, "prime" ) )
  {
    int x = get_atype( argument );

    if( x < APPLY_STR || ( x > APPLY_CON && x != APPLY_LCK ) )
      send_to_char( "Invalid prime attribute!\n\r", ch );
    else
    {
      class->attr_prime = x;
      send_to_char( "Done.\n\r", ch );
    }
    return;
  }
  if( !str_cmp( arg2, "weapon" ) )
  {
    class->weapon = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "guild" ) )
  {
    class->guild = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "thac0" ) )
  {
    class->thac0_00 = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "thac32" ) )
  {
    class->thac0_32 = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "hpmin" ) )
  {
    class->hp_min = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "hpmax" ) )
  {
    class->hp_max = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "mana" ) )
  {
    if( UPPER( argument[0] ) == 'Y' )
      class->fMana = TRUE;
    else
      class->fMana = FALSE;
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "expbase" ) )
  {
    class->exp_base = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "mtitle" ) )
  {
    char arg3[MAX_INPUT_LENGTH];
    int x;

    argument = one_argument( argument, arg3 );
    if( arg3[0] == '\0' || argument[0] == '\0' )
    {
      send_to_char( "Syntax: setclass <class> mtitle <level> <title>\n\r", ch );
      return;
    }
    if( ( x = atoi( arg3 ) ) < 0 || x > MAX_LEVEL )
    {
      send_to_char( "Invalid level.\n\r", ch );
      return;
    }
    DISPOSE( title_table[cl][x][0] );
    title_table[cl][x][0] = str_dup( argument );  /* changed x-1 to x */
    send_to_char( "Done.\n\r", ch );  /* tell user it is done */
    return; /* we are done, so return */
  }
  if( !str_cmp( arg2, "ftitle" ) )
  {
    char arg3[MAX_INPUT_LENGTH];
    char arg4[MAX_INPUT_LENGTH];
    int x, sex;

    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    if( arg3[0] == '\0' || argument[0] == '\0' )
    {
      send_to_char( "Syntax: setclass <class> ftitle <level> <male/female> <title>\n\r", ch );
      return;
    }
    if( ( x = atoi( arg3 ) ) < 0 || x > MAX_LEVEL ) /* changed arg4 to arg3 */
    {
      send_to_char( "Invalid level.\n\r", ch );
      return;
    }
    if( !str_cmp( arg4, "Male" ) )
      sex = 0;
    else
      sex = 1;
    DISPOSE( title_table[cl][x][sex] );
    /*
     * Bug fix below -Shaddai
     */
    title_table[cl][x][sex] = str_dup( argument );
    send_to_char( "Done.\n\r", ch );  /* tell user we are done again */
    return; /* we are done, so return */
  }
  do_setclass( ch, "" );
}


/*
 * Create an instance of a new race.			-Shaddai
 */

bool create_new_race( int index, char *argument )
{
  int i = 0;
  if( index >= MAX_RACE || race_table[index] == NULL )
    return FALSE;
  for( i = 0; i < MAX_WHERE_NAME; i++ )
    race_table[index]->where_name[i] = where_name[i];
  if( argument[0] != '\0' )
    argument[0] = UPPER( argument[0] );
  sprintf( race_table[index]->race_name, "%-.16s", argument );
  race_table[index]->class_restriction = 0;
  race_table[index]->str_plus = 0;
  race_table[index]->dex_plus = 0;
  race_table[index]->int_plus = 0;
  race_table[index]->con_plus = 0;
  race_table[index]->lck_plus = 0;
  race_table[index]->hit = 0;
  race_table[index]->mana = 0;
  xCLEAR_BITS( race_table[index]->affected );
  race_table[index]->resist = 0;
  race_table[index]->suscept = 0;
  race_table[index]->language = 2;
  race_table[index]->alignment = 0;
  race_table[index]->minalign = -1000;
  race_table[index]->maxalign = 1000;
  race_table[index]->ac_plus = 0;
  race_table[index]->exp_multiplier = 100;
  xCLEAR_BITS( race_table[index]->attacks );
  xCLEAR_BITS( race_table[index]->defenses );
  race_table[index]->height = 66;
  race_table[index]->weight = 150;
  race_table[index]->hunger_mod = 0;
  race_table[index]->thirst_mod = 0;
  race_table[index]->mana_regen = 0;
  race_table[index]->hp_regen = 0;
  race_table[index]->race_recall = 0;
  race_table[index]->max_armor = 10000;
  return TRUE;
}

void do_setrace( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  FILE *fpList;
  char racelist[256];
  int value;

  //    char buf[MAX_STRING_LENGTH];
  struct race_type *race;
  int ra, i;

  set_char_color( AT_PLAIN, ch );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if( arg1[0] == '\0' || arg2[0] == '\0' )
  {
    send_to_char( "Syntax: setrace <race> <field> <value>\n\r", ch );
    send_to_char( "Syntax: setrace <race> create	     \n\r", ch );
    send_to_char( "\n\rField being one of:\n\r", ch );
    send_to_char( "  name classes strplus dexplus wisplus\n\r", ch );
    send_to_char( "  intplus conplus chaplus lckplus hit\n\r", ch );
    send_to_char( "  mana affected resist suscept language\n\r", ch );
    send_to_char( "  save attack defense alignment acplus \n\r", ch );
    send_to_char( "  minalign maxalign height weight      \n\r", ch );
    send_to_char( "  hungermod thirstmod expmultiplier    \n\r", ch );
    send_to_char( "  saving_poison_death saving_wand      \n\r", ch );
    send_to_char( "  saving_para_petri saving_breath      \n\r", ch );
    send_to_char( "  saving_spell_staff race_recall       \n\r", ch );
    send_to_char( "  mana_regen hp_regen maxarmor         \n\r", ch );
    return;
  }
  if( is_number( arg1 ) && ( ra = atoi( arg1 ) ) >= 0 && ra < MAX_RACE )
    race = race_table[ra];
  else
  {
    race = NULL;
    for( ra = 0; ra < MAX_RACE && race_table[ra]; ra++ )
    {
      if( !race_table[ra]->race_name )
        continue;
      if( !str_cmp( race_table[ra]->race_name, arg1 ) )
      {
        race = race_table[ra];
        break;
      }
    }
  }
  if( !str_cmp( arg2, "create" ) && race )
  {
    send_to_char( "That race already exists!\n\r", ch );
    return;
  }
  else if( !race && str_cmp( arg2, "create" ) )
  {
    send_to_char( "No such race.\n\r", ch );
    return;
  }



  if( !str_cmp( arg2, "save" ) )
  {
    write_race_file( ra );
    send_to_char( "Saved.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "create" ) )
  {
    if( MAX_PC_RACE >= MAX_RACE )
    {
      send_to_char( "You need to up MAX_RACE in mud.h and make clean.\n\r", ch );
      return;
    }
    if( ( create_new_race( MAX_PC_RACE, arg1 ) ) == FALSE )
    {
      send_to_char( "Couldn't create a new race.\n\r", ch );
      return;
    }
    write_race_file( MAX_PC_RACE );
    MAX_PC_RACE++;
    sprintf( racelist, "%s%s", RACEDIR, RACE_LIST );
    if( ( fpList = fopen( racelist, "w" ) ) == NULL )
    {
      bug( "Error opening racelist.", 0 );
      return;
    }
    for( i = 0; i < MAX_PC_RACE; i++ )
      fprintf( fpList, "%s%s.race\n", RACEDIR, race_table[i]->race_name );
    fprintf( fpList, "$\n" );
    fclose( fpList );
    send_to_char( "Done.\n\r", ch );
    return;
  }

  if( !argument )
  {
    send_to_char( "You must specify an argument.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "name" ) )
  {
    sprintf( race->race_name, "%-.16s", capitalize( argument ) );
    send_to_char( "Done.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "strplus" ) )
  {
    race->str_plus = ( sh_int ) atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "dexplus" ) )
  {
    race->dex_plus = ( sh_int ) atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "intplus" ) )
  {
    race->int_plus = ( sh_int ) atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "conplus" ) )
  {
    race->con_plus = ( sh_int ) atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "lckplus" ) )
  {
    race->lck_plus = ( sh_int ) atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "hit" ) )
  {
    race->hit = ( sh_int ) atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "mana" ) )
  {
    race->mana = ( sh_int ) atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "affected" ) )
  {
    if( !argument || argument[0] == '\0' )
    {
      send_to_char( "Usage: setrace <race> affected <flag> [flag]...\n\r", ch );
      return;
    }
    while( argument[0] != '\0' )
    {
      argument = one_argument( argument, arg3 );
      value = get_aflag( arg3 );
      if( value < 0 || value > MAX_BITS )
        ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
      else
        xTOGGLE_BIT( race->affected, value );
    }
    send_to_char( "Done.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "resist" ) )
  {
    if( !argument || argument[0] == '\0' )
    {
      send_to_char( "Usage: setrace <race> resist <flag> [flag]...\n\r", ch );
      return;
    }
    while( argument[0] != '\0' )
    {
      argument = one_argument( argument, arg3 );
      value = get_risflag( arg3 );
      if( value < 0 || value > 31 )
        ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
      else
        TOGGLE_BIT( race->resist, 1 << value );
    }
    send_to_char( "Done.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "suscept" ) )
  {
    if( !argument || argument[0] == '\0' )
    {
      send_to_char( "Usage: setrace <race> suscept <flag> [flag]...\n\r", ch );
      return;
    }
    while( argument[0] != '\0' )
    {
      argument = one_argument( argument, arg3 );
      value = get_risflag( arg3 );
      if( value < 0 || value > 31 )
        ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
      else
        TOGGLE_BIT( race->suscept, 1 << value );
    }
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "language" ) )
  {
    race->language = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "classes" ) )
  {
    for( i = 0; i < MAX_CLASS; i++ )
    {
      if( !str_cmp( argument, class_table[i]->who_name ) )
      {
        TOGGLE_BIT( race->class_restriction, 1 << i );  /* k, that's boggling */
        send_to_char( "Done.\n\r", ch );
        return;
      }
    }
    send_to_char( "No such class.\n\r", ch );
    return;
  }


  if( !str_cmp( arg2, "acplus" ) )
  {
    race->ac_plus = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "alignment" ) )
  {
    race->alignment = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }

  /*
   * not implemented 
   */
  if( !str_cmp( arg2, "defense" ) )
  {
    if( !argument || argument[0] == '\0' )
    {
      send_to_char( "Usage: setrace <race> defense <flag> [flag]...\n\r", ch );
      return;
    }
    while( argument[0] != '\0' )
    {
      argument = one_argument( argument, arg3 );
      value = get_defenseflag( arg3 );
      if( value < 0 || value > MAX_BITS )
        ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
      else
        xTOGGLE_BIT( race->defenses, value );
    }
    return;
  }

  /*
   * not implemented 
   */
  if( !str_cmp( arg2, "attack" ) )
  {
    if( !argument || argument[0] == '\0' )
    {
      send_to_char( "Usage: setrace <race> attack <flag> [flag]...\n\r", ch );
      return;
    }
    while( argument[0] != '\0' )
    {
      argument = one_argument( argument, arg3 );
      value = get_attackflag( arg3 );
      if( value < 0 || value > MAX_BITS )
        ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
      else
        xTOGGLE_BIT( race->attacks, value );
    }
    return;
  }


  if( !str_cmp( arg2, "minalign" ) )
  {
    race->minalign = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "maxalign" ) )
  {
    race->maxalign = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "height" ) )
  {
    race->height = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "weight" ) )
  {
    race->weight = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "thirstmod" ) )
  {
    race->thirst_mod = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "hungermod" ) )
  {
    race->hunger_mod = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "maxalign" ) )
  {
    race->maxalign = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "expmultiplier" ) )
  {
    race->exp_multiplier = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "saving_poison_death" ) )
  {
    race->saving_poison_death = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "saving_wand" ) )
  {
    race->saving_wand = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "saving_para_petri" ) )
  {
    race->saving_para_petri = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "saving_breath" ) )
  {
    race->saving_breath = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "saving_spell_staff" ) )
  {
    race->saving_spell_staff = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  /*
   * unimplemented stuff follows 
   */
  if( !str_cmp( arg2, "mana_regen" ) )
  {
    race->mana_regen = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "hp_regen" ) )
  {
    race->hp_regen = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "race_recall" ) )
  {
    race->race_recall = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "maxarmor" ) )
  {
    race->max_armor = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }

#ifdef NEW_RACE_STUFF
  if( !str_cmp( arg2, "carry_weight" ) )
  {
    race->acplus = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "carry_number" ) )
  {
    race->acplus = atoi( argument );
    send_to_char( "Done.\n\r", ch );
    return;
  }
#endif
  do_setrace( ch, "" );

}


void do_showrace( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  struct race_type *race;
  int ra, i, ct;

  set_pager_color( AT_PLAIN, ch );

  argument = one_argument( argument, arg1 );

  if( arg1[0] == '\0' )
  {
    send_to_char( "Syntax: showrace  \n\r", ch );
    /*
     * Show the races code addition by Blackmane 
     */
    /*
     * fixed printout by Miki 
     */
    ct = 0;
    for( i = 0; i < MAX_RACE; i++ )
    {
      ++ct;
      pager_printf( ch, "%2d> %-16s", i, race_table[i]->race_name );
      if( ct % 4 == 0 )
        send_to_pager( "\n\r", ch );
    }
    send_to_pager( "\n\r", ch );
    return;
  }

  if( is_number( arg1 ) && ( ra = atoi( arg1 ) ) >= 0 && ra < MAX_RACE )
    race = race_table[ra];
  else
  {
    race = NULL;
    for( ra = 0; ra < MAX_RACE && race_table[ra]; ra++ )
      if( !str_cmp( race_table[ra]->race_name, arg1 ) )
      {
        race = race_table[ra];
        break;
      }
  }
  if( !race )
  {
    send_to_char( "No such race.\n\r", ch );
    return;
  }

  sprintf( buf, "RACE: %s\n\r", race->race_name );
  send_to_char( buf, ch );
  ct = 0;
  sprintf( buf, "Disallowed Classes: " );
  send_to_char( buf, ch );
  for( i = 0; i < MAX_CLASS; i++ )
  {
    if( IS_SET( race->class_restriction, 1 << i ) )
    {
      ct++;
      sprintf( buf, "%s ", class_table[i]->who_name );
      send_to_char( buf, ch );
      if( ct % 6 == 0 )
        send_to_char( "\n\r", ch );
    }
  }
  if( ( ct % 6 != 0 ) || ( ct == 0 ) )
    send_to_char( "\n\r", ch );

  ct = 0;
  sprintf( buf, "Allowed Classes: " );
  send_to_char( buf, ch );
  for( i = 0; i < MAX_CLASS; i++ )
  {
    if( !IS_SET( race->class_restriction, 1 << i ) )
    {
      ct++;
      sprintf( buf, "%s ", class_table[i]->who_name );
      send_to_char( buf, ch );
      if( ct % 6 == 0 )
        send_to_char( "\n\r", ch );
    }
  }
  if( ( ct % 6 != 0 ) || ( ct == 0 ) )
    send_to_char( "\n\r", ch );



  sprintf( buf, "Str Plus: %-3d\tDex Plus: %-3d\tWis Plus: 0\tInt Plus: %-3d\t\n\r",
           race->str_plus, race->dex_plus, race->int_plus );
  send_to_char( buf, ch );
  sprintf( buf, "Con Plus: %-3d\tCha Plus: 0\tLck Plus: %-3d\n\r", race->con_plus, race->lck_plus );
  send_to_char( buf, ch );
  sprintf( buf, "Hit Pts:  %-3d\tMana: %-3d\tAlign: %-4d\tAC: %-d\n\r",
           race->hit, race->mana, race->alignment, race->ac_plus );
  send_to_char( buf, ch );
  sprintf( buf, "Min Align: %d\tMax Align: %-d\t\tXP Mult: %-d%%\n\r",
           race->minalign, race->maxalign, race->exp_multiplier );
  send_to_char( buf, ch );
  sprintf( buf, "Height: %3d in.\t\tWeight: %4d lbs.\tHungerMod: %d\tThirstMod: %d\n\r",
           race->height, race->weight, race->hunger_mod, race->thirst_mod );
  send_to_char( buf, ch );

  send_to_char( "Affected by: ", ch );
  send_to_char( affect_bit_name( &race->affected ), ch );
  send_to_char( "\n\r", ch );

  send_to_char( "Resistant to: ", ch );
  send_to_char( flag_string( race->resist, ris_flags ), ch );
  send_to_char( "\n\r", ch );

  send_to_char( "Susceptible to: ", ch );
  send_to_char( flag_string( race->suscept, ris_flags ), ch );
  send_to_char( "\n\r", ch );

  sprintf( buf, "Saves: (P/D) %d (W) %d (P/P) %d (B) %d (S/S) %d\n\r",
           race->saving_poison_death,
           race->saving_wand, race->saving_para_petri, race->saving_breath, race->saving_spell_staff );
  send_to_char( buf, ch );

  send_to_char( "Innate Attacks: ", ch );
  send_to_char( ext_flag_string( &race->attacks, attack_flags ), ch );
  send_to_char( "\n\r", ch );

  send_to_char( "Innate Defenses: ", ch );
  send_to_char( ext_flag_string( &race->defenses, defense_flags ), ch );
  send_to_char( "\n\r", ch );

}

/*
 * quest point set - TRI
 * syntax is: qpset char give/take amount
 */

void do_qpset( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int amount;
  bool give = TRUE;

  set_char_color( AT_IMMORT, ch );

  if( IS_NPC( ch ) )
  {
    send_to_char( "Cannot qpset as an NPC.\n\r", ch );
    return;
  }
  if( get_trust( ch ) < LEVEL_IMMORTAL )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );
  amount = atoi( arg3 );
  if( arg[0] == '\0' || arg2[0] == '\0' || amount <= 0 )
  {
    send_to_char( "Syntax: qpset <character> <give/take> <amount>\n\r", ch );
    send_to_char( "Amount must be a positive number greater than 0.\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "There is no such player currently playing.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Glory cannot be given to or taken from a mob.\n\r", ch );
    return;
  }

  set_char_color( AT_IMMORT, victim );
  if( nifty_is_name_prefix( arg2, "give" ) )
  {
    give = TRUE;
    if( str_cmp( ch->pcdata->council_name, "Quest Council" ) && ( get_trust( ch ) < LEVEL_DEMI ) )
    {
      send_to_char( "You must be a member of the Quest Council to give qp to a character.\n\r", ch );
      return;
    }
  }
  else if( nifty_is_name_prefix( arg2, "take" ) )
    give = FALSE;
  else
  {
    do_qpset( ch, "" );
    return;
  }

  if( give )
  {
    victim->pcdata->quest_curr += amount;
    victim->pcdata->quest_accum += amount;
    ch_printf( victim, "Your glory has been increased by %d.\n\r", amount );
    ch_printf( ch, "You have increased the glory of %s by %d.\n\r", victim->name, amount );
  }
  else
  {
    if( victim->pcdata->quest_curr - amount < 0 )
    {
      ch_printf( ch, "%s does not have %d glory to take.\n\r", victim->name, amount );
      return;
    }
    else
    {
      victim->pcdata->quest_curr -= amount;
      ch_printf( victim, "Your glory has been decreased by %d.\n\r", amount );
      ch_printf( ch, "You have decreased the glory of %s by %d.\n\r", victim->name, amount );
    }
  }
  return;
}

/* Easy way to check a player's glory -- Blodkai, June 97 */
void do_qpstat( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );

  if( IS_NPC( ch ) )
    return;

  argument = one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Syntax:  qpstat <character>\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "No one by that name currently in the Realms.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "Mobs don't have glory.\n\r", ch );
    return;
  }
  ch_printf( ch, "%s has %d glory, out of a lifetime total of %d.\n\r",
             victim->name, victim->pcdata->quest_curr, victim->pcdata->quest_accum );
  return;
}

/* Simple, small way to make keeping track of small mods easier - Blod */
void do_fixed( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  struct tm *t = localtime( &current_time );

  set_char_color( AT_OBJECT, ch );
  if( argument[0] == '\0' )
  {
    send_to_char( "\n\rUsage:  'fixed list' or 'fixed <message>'", ch );
    if( get_trust( ch ) >= LEVEL_ASCENDANT )
      send_to_char( " or 'fixed clear now'\n\r", ch );
    else
      send_to_char( "\n\r", ch );
    return;
  }
  if( !str_cmp( argument, "clear now" ) && get_trust( ch ) >= LEVEL_ASCENDANT )
  {
    FILE *fp = fopen( FIXED_FILE, "w" );
    if( fp )
      fclose( fp );
    send_to_char( "Fixed file cleared.\n\r", ch );
    return;
  }
  if( !str_cmp( argument, "list" ) )
  {
    send_to_char_color( "\n\r&g[&GDate  &g|  &GVnum&g]\n\r", ch );
    show_file( ch, FIXED_FILE );
  }
  else
  {
    sprintf( buf, "&g|&G%-2.2d/%-2.2d &g| &G%5d&g|  %s:  &G%s",
             t->tm_mon + 1, t->tm_mday, ch->in_room ? ch->in_room->vnum : 0,
             IS_NPC( ch ) ? ch->short_descr : ch->name, argument );
    append_to_file( FIXED_FILE, buf );
    send_to_char( "Thanks, your modification has been logged.\n\r", ch );
  }
  return;
}

void do_fshow( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];

  set_char_color( AT_IMMORT, ch );

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Syntax:  fshow <moblog | plevel>\n\r", ch );
    return;
  }
  if( !str_cmp( arg, "moblog" ) )
  {
    set_char_color( AT_LOG, ch );
    send_to_char( "\n\r[Date_|_Time]  Current moblog:\n\r", ch );
    show_file( ch, MOBLOG_FILE );
    return;
  }
  if( !str_cmp( arg, "plevel" ) )
  {
    set_char_color( AT_NOTE, ch );
    show_file( ch, PLEVEL_FILE );
    return;
  }
  send_to_char( "No such file.\n\r", ch );
  return;
}


RESERVE_DATA *first_reserved;
RESERVE_DATA *last_reserved;
void save_reserved( void )
{
  RESERVE_DATA *res;
  FILE *fp;

  fclose( fpReserve );
  if( !( fp = fopen( SYSTEM_DIR RESERVED_LIST, "w" ) ) )
  {
    bug( "Save_reserved: cannot open " RESERVED_LIST, 0 );
    perror( RESERVED_LIST );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }
  for( res = first_reserved; res; res = res->next )
    fprintf( fp, "%s~\n", res->name );
  fprintf( fp, "$~\n" );
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
  return;
}

/*
 * Command to display the weather status of all the areas
 * Last Modified: July 21, 1997
 * Fireblade
 */
void do_showweather( CHAR_DATA * ch, char *argument )
{
  AREA_DATA *pArea;
  char arg[MAX_INPUT_LENGTH];

  if( !ch )
  {
    bug( "do_showweather: NULL char data" );
    return;
  }

  argument = one_argument( argument, arg );

  set_char_color( AT_BLUE, ch );
  ch_printf( ch, "%-40s%-8s %-8s %-8s\n\r", "Area Name:", "Temp:", "Precip:", "Wind:" );

  for( pArea = first_area; pArea; pArea = pArea->next )
  {
    if( arg[0] == '\0' || nifty_is_name_prefix( arg, pArea->name ) )
    {
      set_char_color( AT_BLUE, ch );
      ch_printf( ch, "%-40s", pArea->name );
      set_char_color( AT_WHITE, ch );
      ch_printf( ch, "%3d", pArea->weather->temp );
      set_char_color( AT_BLUE, ch );
      ch_printf( ch, "(" );
      set_char_color( AT_LBLUE, ch );
      ch_printf( ch, "%3d", pArea->weather->temp_vector );
      set_char_color( AT_BLUE, ch );
      ch_printf( ch, ") " );
      set_char_color( AT_WHITE, ch );
      ch_printf( ch, "%3d", pArea->weather->precip );
      set_char_color( AT_BLUE, ch );
      ch_printf( ch, "(" );
      set_char_color( AT_LBLUE, ch );
      ch_printf( ch, "%3d", pArea->weather->precip_vector );
      set_char_color( AT_BLUE, ch );
      ch_printf( ch, ") " );
      set_char_color( AT_WHITE, ch );
      ch_printf( ch, "%3d", pArea->weather->wind );
      set_char_color( AT_BLUE, ch );
      ch_printf( ch, "(" );
      set_char_color( AT_LBLUE, ch );
      ch_printf( ch, "%3d", pArea->weather->wind_vector );
      set_char_color( AT_BLUE, ch );
      ch_printf( ch, ")\n\r" );
    }
  }

  return;
}

/*
 * Command to control global weather variables and to reset weather
 * Last Modified: July 23, 1997
 * Fireblade
 */
void do_setweather( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];

  set_char_color( AT_BLUE, ch );

  argument = one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    ch_printf( ch, "%-15s%-6s\n\r", "Parameters:", "Value:" );
    ch_printf( ch, "%-15s%-6d\n\r", "random", rand_factor );
    ch_printf( ch, "%-15s%-6d\n\r", "climate", climate_factor );
    ch_printf( ch, "%-15s%-6d\n\r", "neighbor", neigh_factor );
    ch_printf( ch, "%-15s%-6d\n\r", "unit", weath_unit );
    ch_printf( ch, "%-15s%-6d\n\r", "maxvector", max_vector );

    ch_printf( ch, "\n\rResulting values:\n\r" );
    ch_printf( ch, "Weather variables range from " "%d to %d.\n\r", -3 * weath_unit, 3 * weath_unit );
    ch_printf( ch, "Weather vectors range from " "%d to %d.\n\r", -1 * max_vector, max_vector );
    ch_printf( ch, "The maximum a vector can "
               "change in one update is %d.\n\r", rand_factor + 2 * climate_factor + ( 6 * weath_unit / neigh_factor ) );
  }
  else if( !str_cmp( arg, "random" ) )
  {
    if( !is_number( argument ) )
    {
      ch_printf( ch, "Set maximum random " "change in vectors to what?\n\r" );
    }
    else
    {
      rand_factor = atoi( argument );
      ch_printf( ch, "Maximum random " "change in vectors now " "equals %d.\n\r", rand_factor );
      save_weatherdata(  );
    }
  }
  else if( !str_cmp( arg, "climate" ) )
  {
    if( !is_number( argument ) )
    {
      ch_printf( ch, "Set climate effect " "coefficient to what?\n\r" );
    }
    else
    {
      climate_factor = atoi( argument );
      ch_printf( ch, "Climate effect " "coefficient now equals " "%d.\n\r", climate_factor );
      save_weatherdata(  );
    }
  }
  else if( !str_cmp( arg, "neighbor" ) )
  {
    if( !is_number( argument ) )
    {
      ch_printf( ch, "Set neighbor effect " "divisor to what?\n\r" );
    }
    else
    {
      neigh_factor = atoi( argument );

      if( neigh_factor <= 0 )
        neigh_factor = 1;

      ch_printf( ch, "Neighbor effect " "coefficient now equals " "1/%d.\n\r", neigh_factor );
      save_weatherdata(  );
    }
  }
  else if( !str_cmp( arg, "unit" ) )
  {
    if( !is_number( argument ) )
    {
      ch_printf( ch, "Set weather unit " "size to what?\n\r" );
    }
    else
    {
      weath_unit = atoi( argument );
      ch_printf( ch, "Weather unit size " "now equals %d.\n\r", weath_unit );
      save_weatherdata(  );
    }
  }
  else if( !str_cmp( arg, "maxvector" ) )
  {
    if( !is_number( argument ) )
    {
      ch_printf( ch, "Set maximum vector " "size to what?\n\r" );
    }
    else
    {
      max_vector = atoi( argument );
      ch_printf( ch, "Maximum vector size " "now equals %d.\n\r", max_vector );
      save_weatherdata(  );
    }
  }
  else if( !str_cmp( arg, "reset" ) )
  {
    init_area_weather(  );
    ch_printf( ch, "Weather system reinitialized.\n\r" );
  }
  else if( !str_cmp( arg, "update" ) )
  {
    int i, number;

    number = atoi( argument );

    if( number < 1 )
      number = 1;

    for( i = 0; i < number; i++ )
      weather_update(  );

    ch_printf( ch, "Weather system updated.\n\r" );
  }
  else
  {
    ch_printf( ch, "You may only use one of the " "following fields:\n\r" );
    ch_printf( ch, "\trandom\n\r\tclimate\n\r" "\tneighbor\n\r\tunit\n\r\tmaxvector\n\r" );
    ch_printf( ch, "You may also reset or update " "the system using the fields 'reset' " "and 'update' respectively.\n\r" );
  }

  return;
}


void do_khistory( CHAR_DATA * ch, char *argument )
{
  MOB_INDEX_DATA *tmob;
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *vch;
  int track;

  if( IS_NPC( ch ) || !IS_IMMORTAL( ch ) )
  {
    ch_printf( ch, "Huh?\n\r" );
    return;
  }

  one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    ch_printf( ch, "syntax: khistory <player>\n\r" );
    return;
  }

  vch = get_char_world( ch, arg );

  if( !vch || IS_NPC( vch ) )
  {
    ch_printf( ch, "They are not here.\n\r" );
    return;
  }

  set_char_color( AT_BLOOD, ch );
  ch_printf( ch, "Kill history for %s:\n\r", vch->name );

  for( track = 0; track < MAX_KILLTRACK && vch->pcdata->killed[track].vnum; track++ )
  {
    tmob = get_mob_index( vch->pcdata->killed[track].vnum );

    if( !tmob )
    {
      bug( "killhistory: unknown mob vnum" );
      continue;
    }

    set_char_color( AT_RED, ch );
    ch_printf( ch, "   %-30s", capitalize( tmob->short_descr ) );
    set_char_color( AT_BLOOD, ch );
    ch_printf( ch, "(" );
    set_char_color( AT_RED, ch );
    ch_printf( ch, "%-5d", tmob->vnum );
    set_char_color( AT_BLOOD, ch );
    ch_printf( ch, ")" );
    set_char_color( AT_RED, ch );
    ch_printf( ch, "    - killed %d times.\n\r", vch->pcdata->killed[track].count );
  }

  return;
}

void do_project( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  int pcount;
  int pnum;
  PROJECT_DATA *pproject;

  if( IS_NPC( ch ) )
    return;

  if( !ch->desc )
  {
    bug( "do_project: no descriptor", 0 );
    return;
  }

  switch ( ch->substate )
  {
    default:
      break;
    case SUB_WRITING_NOTE:
      if( !ch->pnote )
      {
        bug( "do_project: log got lost?", 0 );
        send_to_char( "Your log was lost!\n\r", ch );
        stop_editing( ch );
        return;
      }
      if( ch->dest_buf != ch->pnote )
        bug( "do_project: sub_writing_note: ch->dest_buf != ch->pnote", 0 );
      STRFREE( ch->pnote->text );
      ch->pnote->text = copy_buffer( ch );
      stop_editing( ch );
      return;
    case SUB_PROJ_DESC:
      if( !ch->dest_buf )
      {
        send_to_char( "Your description was lost!", ch );
        bug( "do_project: sub_project_desc: NULL ch->dest_buf", 0 );
        ch->substate = SUB_NONE;
        return;
      }
      pproject = ch->dest_buf;
      STRFREE( pproject->description );
      pproject->description = copy_buffer( ch );
      stop_editing( ch );
      ch->substate = ch->tempnum;
      write_projects(  );
      return;
  }

  set_char_color( AT_NOTE, ch );
  argument = one_argument( argument, arg );
  smash_tilde( argument );

  if( !str_cmp( arg, "save" ) )
  {
    write_projects(  );
    ch_printf( ch, "Projects saved.\n\r" );
    return;
  }

  if( !str_cmp( arg, "code" ) )
  {
    pcount = 0;
    pager_printf( ch, " # | Owner       | Project              |\n\r" );
    pager_printf( ch, "---|-------------|----------------------|--------------------------|-----------\n\r" );
    for( pproject = first_project; pproject; pproject = pproject->next )
    {
      pcount++;
      if( ( pproject->status && str_cmp( pproject->status, "approved" ) ) || pproject->coder != NULL )
        continue;
      pager_printf( ch, "%2d | %-11s | %-20s |\n\r", pcount, pproject->owner ? pproject->owner : "(None)", pproject->name );
    }
    return;
  }
  if( !str_cmp( arg, "more" ) || !str_cmp( arg, "mine" ) )
  {
    NOTE_DATA *log;
    bool MINE = FALSE;
    int num_logs = 0;
    pcount = 0;

    if( !str_cmp( arg, "mine" ) )
      MINE = TRUE;

    pager_printf( ch, "\n\r" );
    pager_printf( ch, " # | Owner       | Project              | Coder         | Status     | # of Logs\n\r" );
    pager_printf( ch, "---|-------------|----------------------|---------------|------------|----------\n\r" );
    for( pproject = first_project; pproject; pproject = pproject->next )
    {
      pcount++;
      if( MINE && ( !pproject->owner || str_cmp( ch->name, pproject->owner ) )
          && ( !pproject->coder || str_cmp( ch->name, pproject->coder ) ) )
        continue;
      else if( !MINE && pproject->status && !str_cmp( "Done", pproject->status ) )
        continue;
      num_logs = 0;
      for( log = pproject->first_log; log; log = log->next )
        num_logs++;
      pager_printf( ch, "%2d | %-11s | %-20s | %-13s | %-10s | %3d\n\r",
                    pcount,
                    pproject->owner ? pproject->owner : "(None)",
                    pproject->name,
                    pproject->coder ? pproject->coder : "(None)", pproject->status ? pproject->status : "(None)", num_logs );
    }
    return;
  }
  if( arg[0] == '\0' || !str_cmp( arg, "list" ) )
  {
    bool aflag, projects_available;
    aflag = FALSE;
    projects_available = FALSE;
    if( !str_cmp( argument, "available" ) )
      aflag = TRUE;

    pager_printf( ch, "\n\r" );
    if( !aflag )
    {
      pager_printf( ch, " # | Owner       | Project              | Date                     | Status\n\r" );
      pager_printf( ch, "---|-------------|----------------------|--------------------------|-----------\n\r" );
    }
    else
    {
      pager_printf( ch, " # | Project              | Date\n\r" );
      pager_printf( ch, "---|----------------------|--------------------------\n\r" );
    }
    pcount = 0;
    for( pproject = first_project; pproject; pproject = pproject->next )
    {
      pcount++;
      if( pproject->status && !str_cmp( "Done", pproject->status ) )
        continue;
      if( !aflag )
        pager_printf( ch, "%2d | %-11s | %-20s | %-24s | %-10s\n\r",
                      pcount,
                      pproject->owner ? pproject->owner : "(None)",
                      pproject->name, pproject->date, pproject->status ? pproject->status : "(None)" );
      else if( !pproject->taken )
      {
        if( !projects_available )
          projects_available = TRUE;
        pager_printf( ch, "%2d | %-20s | %s\n\r", pcount, pproject->name, pproject->date );
      }
    }
    if( pcount == 0 )
      pager_printf( ch, "No projects exist.\n\r" );
    else if( aflag && !projects_available )
      pager_printf( ch, "No projects available.\n\r" );
    return;
  }

  if( !str_cmp( arg, "add" ) )
  {
    char *strtime;
    PROJECT_DATA *new_project;  /* Just to be safe */

    if( get_trust( ch ) < LEVEL_GOD && str_cmp( ch->pcdata->council_name, "Code Council" ) )
    {
      send_to_char( "You are not powerfull enough to add a new project.\n\r", ch );
      return;
    }

    CREATE( new_project, PROJECT_DATA, 1 );
    LINK( new_project, first_project, last_project, next, prev );
    new_project->name = str_dup( argument );
    new_project->coder = NULL;
    new_project->taken = FALSE;
    new_project->description = STRALLOC( "" );
    strtime = ctime( &current_time );
    strtime[strlen( strtime ) - 1] = '\0';
    new_project->date = STRALLOC( strtime );
    write_projects(  );
    ch_printf( ch, "Ok.\n\r" );
    return;
  }

  if( !is_number( arg ) )
  {
    ch_printf( ch, "Invalid project.\n\r" );
    return;
  }

  pnum = atoi( arg );
  pproject = get_project_by_number( pnum );
  if( !pproject )
  {
    ch_printf( ch, "No such project.\n\r" );
    return;
  }

  argument = one_argument( argument, arg1 );

  if( !str_cmp( arg1, "description" ) )
  {
    if( get_trust( ch ) < LEVEL_GOD && str_cmp( ch->pcdata->council_name, "Code Council" ) )
      CHECK_SUBRESTRICTED( ch );
    ch->tempnum = SUB_NONE;
    ch->substate = SUB_PROJ_DESC;
    ch->dest_buf = pproject;
    if( pproject->description == NULL )
      pproject->description = STRALLOC( "" );
    start_editing( ch, pproject->description );
    return;
  }
  if( !str_cmp( arg1, "delete" ) )
  {
    NOTE_DATA *log, *tlog;
    if( str_cmp( ch->pcdata->council_name, "Code Council" ) && get_trust( ch ) < LEVEL_ASCENDANT )
    {
      send_to_char( "You are not high enough level to delete a project.\n\r", ch );
      return;
    }

    log = pproject->last_log;
    while( log )
    {
      UNLINK( log, pproject->first_log, pproject->last_log, next, prev );
      tlog = log->prev;
      free_note( log );
      log = tlog;
    }
    UNLINK( pproject, first_project, last_project, next, prev );

    DISPOSE( pproject->name );
    if( pproject->coder )
      DISPOSE( pproject->coder );
    if( pproject->owner )
      STRFREE( pproject->owner );
    if( pproject->description )
      STRFREE( pproject->description );
    if( pproject->date )
      STRFREE( pproject->date );
    if( pproject->status )
      STRFREE( pproject->status );

    DISPOSE( pproject );
    write_projects(  );
    ch_printf( ch, "Ok.\n\r" );
    return;
  }

  if( !str_cmp( arg1, "take" ) )
  {
    if( pproject->taken && pproject->owner && !str_cmp( pproject->owner, ch->name ) )
    {
      pproject->taken = FALSE;
      STRFREE( pproject->owner );
      pproject->owner = NULL;
      send_to_char( "You removed yourself as the owner.\n\r", ch );
      write_projects(  );
      return;
    }
    else if( pproject->taken )
    {
      ch_printf( ch, "This project is already taken.\n\r" );
      return;
    }


    if( pproject->owner )
      STRFREE( pproject->owner );
    pproject->owner = STRALLOC( ch->name );
    pproject->taken = TRUE;
    write_projects(  );
    ch_printf( ch, "Ok.\n\r" );
    return;
  }
  if( !str_cmp( arg1, "coder" ) )
  {
    if( pproject->coder && !str_cmp( ch->name, pproject->coder ) )
    {
      DISPOSE( pproject->coder );
      pproject->coder = NULL;
      send_to_char( "You removed yourself as the coder.\n\r", ch );
      write_projects(  );
      return;
    }
    else if( pproject->coder )
    {
      ch_printf( ch, "This project already has a coder.\n\r" );
      return;
    }
    pproject->coder = str_dup( ch->name );
    write_projects(  );
    ch_printf( ch, "Ok.\n\r" );
    return;
  }
  if( !str_cmp( arg1, "status" ) )
  {
    if( pproject->owner && str_cmp( pproject->owner, ch->name ) &&
        get_trust( ch ) < LEVEL_GREATER
        && pproject->coder && str_cmp( pproject->coder, ch->name ) && str_cmp( ch->pcdata->council_name, "Code Council" ) )
    {
      ch_printf( ch, "This is not your project!\n\r" );
      return;
    }
    if( pproject->status )
      STRFREE( pproject->status );
    pproject->status = STRALLOC( argument );
    write_projects(  );
    send_to_char( "Done.\n\r", ch );
    return;
  }
  if( !str_cmp( arg1, "show" ) )
  {
    if( pproject->description )
      send_to_char( pproject->description, ch );
    else
      send_to_char( "That project does not have a description.\n\r", ch );
    return;
  }
  if( !str_cmp( arg1, "log" ) )
  {
    NOTE_DATA *plog;
    if( !str_cmp( argument, "write" ) )
    {
      note_attach( ch );
      ch->substate = SUB_WRITING_NOTE;
      ch->dest_buf = ch->pnote;
      start_editing( ch, ch->pnote->text );
      return;
    }

    argument = one_argument( argument, arg2 );

    if( !str_cmp( arg2, "subject" ) )
    {
      note_attach( ch );
      STRFREE( ch->pnote->subject );
      ch->pnote->subject = STRALLOC( argument );
      ch_printf( ch, "Ok.\n\r" );
      return;
    }

    if( !str_cmp( arg2, "post" ) )
    {
      char *strtime;

      if( pproject->owner && str_cmp( ch->name, pproject->owner ) &&
          pproject->coder && str_cmp( ch->name, pproject->coder ) &&
          get_trust( ch ) < LEVEL_GREATER && str_cmp( ch->pcdata->council_name, "Code Council" ) )
      {
        ch_printf( ch, "This is not your project!\n\r" );
        return;
      }

      if( !ch->pnote )
      {
        ch_printf( ch, "You have no log in progress.\n\r" );
        return;
      }

      if( !ch->pnote->subject )
      {
        ch_printf( ch, "Your log has no subject.\n\r" );
        return;
      }

      strtime = ctime( &current_time );
      strtime[strlen( strtime ) - 1] = '\0';
      ch->pnote->date = STRALLOC( strtime );
      ch->pnote->sender = ch->name;

      plog = ch->pnote;
      ch->pnote = NULL;
      LINK( plog, pproject->first_log, pproject->last_log, next, prev );
      write_projects(  );
      ch_printf( ch, "Ok.\n\r" );
      return;
    }

    if( !str_cmp( arg2, "list" ) )
    {
      if( pproject->owner && pproject->coder &&
          str_cmp( ch->name, pproject->owner ) && get_trust( ch ) < LEVEL_SAVIOR
          && str_cmp( ch->name, pproject->coder ) && str_cmp( ch->pcdata->council_name, "Code Council" ) )
      {
        ch_printf( ch, "This is not your project!\n\r" );
        return;
      }

      pcount = 0;
      pager_printf( ch, "Project: %-12s: %s\n\r", pproject->owner ? pproject->owner : "(None)", pproject->name );

      for( plog = pproject->first_log; plog; plog = plog->next )
      {
        pcount++;
        pager_printf( ch, "%2d) %-12s: %s\n\r", pcount, plog->sender, plog->subject );
      }
      if( pcount == 0 )
        ch_printf( ch, "No logs available.\n\r" );
      return;
    }

    if( !is_number( arg2 ) )
    {
      ch_printf( ch, "Invalid log.\n\r" );
      return;
    }

    pnum = atoi( arg2 );

    plog = get_log_by_number( pproject, pnum );
    if( !plog )
    {
      ch_printf( ch, "Invalid log.\n\r" );
      return;
    }


    if( !str_cmp( argument, "delete" ) )
    {
      if( pproject->owner && str_cmp( ch->name, pproject->owner ) &&
          get_trust( ch ) < LEVEL_ASCENDANT &&
          pproject->coder && str_cmp( ch->name, pproject->coder ) && str_cmp( ch->pcdata->council_name, "Code Council" ) )
      {
        ch_printf( ch, "This is not your project!\n\r" );
        return;
      }

      UNLINK( plog, pproject->first_log, pproject->last_log, next, prev );
      free_note( plog );
      write_projects(  );
      ch_printf( ch, "Ok.\n\r" );
      return;
    }

    if( !str_cmp( argument, "read" ) )
    {
      if( pproject->owner && pproject->coder &&
          str_cmp( ch->name, pproject->owner ) && get_trust( ch ) < LEVEL_SAVIOR
          && str_cmp( ch->name, pproject->coder ) && str_cmp( ch->pcdata->council_name, "Code Council" ) )
      {
        ch_printf( ch, "This is not your project!\n\r" );
        return;
      }

      pager_printf( ch, "[%3d] %s: %s\n\r%s\n\r%s", pnum, plog->sender, plog->subject, plog->date, plog->text );
      return;
    }
  }
  send_to_char( "Unknown syntax see help 'PROJECT'.\n\r", ch );
  return;
}

PROJECT_DATA *get_project_by_number( int pnum )
{
  int pcount;
  PROJECT_DATA *pproject;
  pcount = 0;
  for( pproject = first_project; pproject; pproject = pproject->next )
  {
    pcount++;
    if( pcount == pnum )
      return pproject;
  }
  return NULL;
}

NOTE_DATA *get_log_by_number( PROJECT_DATA * pproject, int pnum )
{
  int pcount;
  NOTE_DATA *plog;
  pcount = 0;
  for( plog = pproject->first_log; plog; plog = plog->next )
  {
    pcount++;
    if( pcount == pnum )
      return plog;
  }
  return NULL;
}

/*
 * Command to check for multiple ip addresses in the mud.
 * --Shaddai
 */

/*
 * Added this new struct to do matching
 * If ya think of a better way do it, easiest way I could think of at
 * 2 in the morning :) --Shaddai
 */

typedef struct ipcompare_data IPCOMPARE_DATA;
struct ipcompare_data
{
  struct ipcompare_data *prev;
  struct ipcompare_data *next;
  char *host;
  char *name;
  char *user;
  int connected;
  int count;
  int descriptor;
  int idle;
  int port;
  bool printed;
};

void do_ipcompare( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  DESCRIPTOR_DATA *d;
  char arg[MAX_INPUT_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char *addie = NULL;
  bool prefix = FALSE, suffix = FALSE, inarea = FALSE, inroom = FALSE, inworld = FALSE;
  int count = 0, times = -1;
  bool fMatch;
  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  set_pager_color( AT_PLAIN, ch );

  if( IS_NPC( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }

  if( arg[0] == '\0' )
  {
    send_to_char( "ipcompare pkill\n\r", ch );
    send_to_char( "ipcompare total\n\r", ch );
    send_to_char( "ipcompare <person> [room|area|world] [#]\n\r", ch );
    send_to_char( "ipcompare <site>   [room|area|world] [#]\n\r", ch );
    return;
  }
  if( !str_cmp( arg, "total" ) )
  {
    IPCOMPARE_DATA *first_ip = NULL, *last_ip = NULL, *hmm, *hmm_next;
    for( d = first_descriptor; d; d = d->next )
    {
      fMatch = FALSE;
      for( hmm = first_ip; hmm; hmm = hmm->next )
        if( !str_cmp( hmm->host, d->host ) )
          fMatch = TRUE;
      if( !fMatch )
      {
        IPCOMPARE_DATA *temp;
        CREATE( temp, IPCOMPARE_DATA, 1 );
        temp->host = str_dup( d->host );
        LINK( temp, first_ip, last_ip, next, prev );
        count++;
      }
    }
    for( hmm = first_ip; hmm; hmm = hmm_next )
    {
      hmm_next = hmm->next;
      UNLINK( hmm, first_ip, last_ip, next, prev );
      if( hmm->host )
        DISPOSE( hmm->host );
      DISPOSE( hmm );
    }
    ch_printf( ch, "There were %d unique ip addresses found.\n\r", count );
    return;
  }
  else if( !str_cmp( arg, "pkill" ) )
  {
    IPCOMPARE_DATA *first_ip = NULL, *last_ip = NULL, *hmm, *hmm_next;
    sprintf( buf, "\n\rDesc|Con|Idle| Port | Player      " );
    if( get_trust( ch ) >= LEVEL_SAVIOR )
      strcat( buf, "@HostIP           " );
    if( get_trust( ch ) >= LEVEL_GOD )
      strcat( buf, "| Username" );
    strcat( buf, "\n\r" );
    strcat( buf, "----+---+----+------+-------------" );
    if( get_trust( ch ) >= LEVEL_SAVIOR )
      strcat( buf, "------------------" );
    if( get_trust( ch ) >= LEVEL_GOD )
      strcat( buf, "+---------" );
    strcat( buf, "\n\r" );
    send_to_pager( buf, ch );

    for( d = first_descriptor; d; d = d->next )
    {
      IPCOMPARE_DATA *temp;

      if( ( d->connected != CON_PLAYING && d->connected != CON_EDITING )
          || d->character == NULL || !CAN_PKILL( d->character ) || !can_see( ch, d->character ) )
        continue;
      CREATE( temp, IPCOMPARE_DATA, 1 );
      temp->host = str_dup( d->host );
      temp->descriptor = d->descriptor;
      temp->connected = d->connected;
      temp->idle = d->idle;
      temp->port = d->port;
      temp->name = ( d->original ? str_dup( d->original->name ) :
                     d->character ? str_dup( d->character->name ) : str_dup( "(none)" ) );
      temp->user = str_dup( d->user );
      temp->count = 0;
      temp->printed = FALSE;
      LINK( temp, first_ip, last_ip, next, prev );
    }

    for( d = first_descriptor; d; d = d->next )
    {
      fMatch = FALSE;
      if( ( d->connected != CON_PLAYING && d->connected != CON_EDITING )
          || d->character == NULL || !can_see( ch, d->character ) )
        continue;
      for( hmm = first_ip; hmm; hmm = hmm->next )
      {
        if( !str_cmp( hmm->host, d->host ) &&
            str_cmp( hmm->name, ( d->original ? d->original->name : d->character ? d->character->name : "(none)" ) ) )
        {
          fMatch = TRUE;
          break;
        }
      }
      if( fMatch && hmm )
      {
        hmm->count++;
        if( !hmm->printed && hmm->count > 0 )
        {
          sprintf( buf, " %3d| %2d|%4d|%6d| %-12s", hmm->descriptor, hmm->connected, hmm->idle / 4, hmm->port, hmm->name );
          if( get_trust( ch ) >= LEVEL_SAVIOR )
            sprintf( buf + strlen( buf ), "@%-16s ", hmm->host );
          if( get_trust( ch ) >= LEVEL_GOD )
            sprintf( buf + strlen( buf ), "| %s", hmm->user );
          strcat( buf, "\n\r" );
          send_to_pager( buf, ch );
          hmm->printed = TRUE;
        }
        sprintf( buf,
                 " %3d| %2d|%4d|%6d| %-12s",
                 d->descriptor,
                 d->connected,
                 d->idle / 4, d->port, d->original ? d->original->name : d->character ? d->character->name : "(none)" );
        if( get_trust( ch ) >= LEVEL_SAVIOR )
          sprintf( buf + strlen( buf ), "@%-16s ", d->host );
        if( get_trust( ch ) >= LEVEL_GOD )
          sprintf( buf + strlen( buf ), "| %s", d->user );
        strcat( buf, "\n\r" );
        send_to_pager( buf, ch );
      }
    }
    for( hmm = first_ip; hmm; hmm = hmm_next )
    {
      hmm_next = hmm->next;
      UNLINK( hmm, first_ip, last_ip, next, prev );
      if( hmm->name )
        DISPOSE( hmm->name );
      if( hmm->host )
        DISPOSE( hmm->host );
      if( hmm->user )
        DISPOSE( hmm->user );
      DISPOSE( hmm );
    }
    return;
  }
  if( arg1[0] != '\0' )
  {
    if( is_number( arg1 ) )
      times = atoi( arg1 );
    else
    {
      if( !str_cmp( arg1, "room" ) )
        inroom = TRUE;
      else if( !str_cmp( arg1, "area" ) )
        inarea = TRUE;
      else
        inworld = TRUE;
    }
    if( arg2[0] != '\0' )
    {
      if( is_number( arg2 ) )
        times = atoi( arg2 );
      else
      {
        send_to_char( "Please see help ipcompare for more info.\n\r", ch );
        return;
      }
    }
  }
  if( ( victim = get_char_world( ch, arg ) ) != NULL && victim->desc )
  {
    if( IS_NPC( victim ) )
    {
      send_to_char( "Not on NPC's.\n\r", ch );
      return;
    }
    addie = victim->desc->host;
  }
  else
  {
    addie = arg;
    if( arg[0] == '*' )
    {
      prefix = TRUE;
      addie++;
    }
    if( addie[strlen( addie ) - 1] == '*' )
    {
      suffix = TRUE;
      addie[strlen( addie ) - 1] = '\0';
    }
  }
  sprintf( buf, "\n\rDesc|Con|Idle| Port | Player      " );
  if( get_trust( ch ) >= LEVEL_SAVIOR )
    strcat( buf, "@HostIP           " );
  if( get_trust( ch ) >= LEVEL_GOD )
    strcat( buf, "| Username" );
  strcat( buf, "\n\r" );
  strcat( buf, "----+---+----+------+-------------" );
  if( get_trust( ch ) >= LEVEL_SAVIOR )
    strcat( buf, "------------------" );
  if( get_trust( ch ) >= LEVEL_GOD )
    strcat( buf, "+---------" );
  strcat( buf, "\n\r" );
  send_to_pager( buf, ch );
  for( d = first_descriptor; d; d = d->next )
  {
    if( !d->character || ( d->connected != CON_PLAYING && d->connected != CON_EDITING ) || !can_see( ch, d->character ) )
      continue;
    if( inroom && ch->in_room != d->character->in_room )
      continue;
    if( inarea && ch->in_room->area != d->character->in_room->area )
      continue;
    if( times > 0 && count == ( times - 1 ) )
      break;
    if( prefix && suffix && strstr( addie, d->host ) )
      fMatch = TRUE;
    else if( prefix && !str_suffix( addie, d->host ) )
      fMatch = TRUE;
    else if( suffix && !str_prefix( addie, d->host ) )
      fMatch = TRUE;
    else if( !str_cmp( d->host, addie ) )
      fMatch = TRUE;
    else
      fMatch = FALSE;
    if( fMatch )
    {
      count++;
      sprintf( buf,
               " %3d| %2d|%4d|%6d| %-12s",
               d->descriptor,
               d->connected,
               d->idle / 4, d->port, d->original ? d->original->name : d->character ? d->character->name : "(none)" );
      if( get_trust( ch ) >= LEVEL_SAVIOR )
        sprintf( buf + strlen( buf ), "@%-16s ", d->host );
      if( get_trust( ch ) >= LEVEL_GOD )
        sprintf( buf + strlen( buf ), "| %s", d->user );
      strcat( buf, "\n\r" );
      send_to_pager( buf, ch );
    }
  }
  pager_printf( ch, "%d user%s.\n\r", count, count == 1 ? "" : "s" );
  return;
}


/*
 * New nuisance flag to annoy people that deserve it :) --Shaddai
 */
void do_nuisance( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  struct tm *now_time;
  int time = 0, max_time = 0, power = 1;
  bool minute = FALSE, day = FALSE, hour = FALSE;

  if( IS_NPC( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    send_to_char( "Syntax: nuisance <victim> [Options]\n\r", ch );
    send_to_char( "Options:\n\r", ch );
    send_to_char( "  power <level 1-10>\n\r", ch );
    send_to_char( "  time  <days>\n\r", ch );
    send_to_char( "  maxtime <#> <minutes/hours/days>\n\r", ch );
    send_to_char( "Defaults: Time -- forever, power -- 1, maxtime 8 days.\n\r", ch );
    return;
  }

  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "There is no one on with that name.\n\r", ch );
    return;
  }

  if( IS_NPC( victim ) )
  {
    send_to_char( "You can't set a nuisance flag on a mob.\n\r", ch );
    return;
  }

  if( get_trust( ch ) <= get_trust( victim ) )
  {
    send_to_char( "I don't think they would like that.\n\r", ch );
    return;
  }

  if( victim->pcdata->nuisance )
  {
    send_to_char( "That flag has already been set.\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg1 );

  while( argument[0] != '\0' )
  {
    if( !str_cmp( arg1, "power" ) )
    {
      argument = one_argument( argument, arg1 );
      if( arg1[0] == '\0' || !is_number( arg1 ) )
      {
        send_to_char( "Power option syntax: power <number>\n\r", ch );
        return;
      }
      if( ( power = atoi( arg1 ) ) < 1 || power > 10 )
      {
        send_to_char( "Power must be 1 - 10.\n\r", ch );
        return;
      }
    }
    else if( !str_cmp( arg1, "time" ) )
    {
      argument = one_argument( argument, arg1 );
      if( arg1[0] == '\0' || !is_number( arg1 ) )
      {
        send_to_char( "Time option syntax: time <number> (In days)\n\r", ch );
        return;
      }
      if( ( time = atoi( arg1 ) ) < 1 )
      {
        send_to_char( "Time must be a positive number.\n\r", ch );
        return;
      }
    }
    else if( !str_cmp( arg1, "maxtime" ) )
    {
      argument = one_argument( argument, arg1 );
      argument = one_argument( argument, arg2 );
      if( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg1 ) )
      {
        send_to_char( "Maxtime option syntax: maxtime <number> <minute|day|hour>\n\r", ch );
        return;
      }
      if( ( max_time = atoi( arg1 ) ) < 1 )
      {
        send_to_char( "Maxtime must be a positive number.\n\r", ch );
        return;
      }
      if( !str_cmp( arg2, "minutes" ) )
        minute = TRUE;
      else if( !str_cmp( arg2, "hours" ) )
        hour = TRUE;
      else if( !str_cmp( arg2, "days" ) )
        day = TRUE;
    }
    else
    {
      ch_printf( ch, "Unknown option %s.\n\r", arg1 );
      return;
    }
    argument = one_argument( argument, arg1 );
  }

  if( minute && ( max_time < 1 || max_time > 59 ) )
  {
    send_to_char( "Minutes must be 1 to 59.\n\r", ch );
    return;
  }
  else if( hour && ( max_time < 1 || max_time > 23 ) )
  {
    send_to_char( "Hours must be 1 - 23.\n\r", ch );
    return;
  }
  else if( day && ( max_time < 1 || max_time > 999 ) )
  {
    send_to_char( "Days must be 1 - 999.\n\r", ch );
    return;
  }
  else if( !max_time )
  {
    day = TRUE;
    max_time = 7;
  }
  CREATE( victim->pcdata->nuisance, NUISANCE_DATA, 1 );
  victim->pcdata->nuisance->time = current_time;
  victim->pcdata->nuisance->flags = 1;
  victim->pcdata->nuisance->power = power;
  now_time = localtime( &current_time );

  if( minute )
    now_time->tm_min += max_time;
  else if( hour )
    now_time->tm_hour += max_time;
  else
    now_time->tm_mday += max_time;

  victim->pcdata->nuisance->max_time = mktime( now_time );
  if( time )
  {
    add_timer( victim, TIMER_NUISANCE, ( 28800 * time ), NULL, 0 );
    ch_printf( ch, "Nuisance flag set for %d days.\n\r", time );
  }
  else
    send_to_char( "Nuisance flag set forever\n\r", ch );
  return;
}

void do_unnuisance( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  TIMER *timer, *timer_next;
  char arg[MAX_INPUT_LENGTH];

  if( IS_NPC( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  one_argument( argument, arg );

  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "There is no one on with that name.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "You can't remove a nuisance flag from a mob.\n\r", ch );
    return;
  }
  if( get_trust( ch ) <= get_trust( victim ) )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }
  if( !victim->pcdata->nuisance )
  {
    send_to_char( "They do not have that flag set.\n\r", ch );
    return;
  }
  for( timer = victim->first_timer; timer; timer = timer_next )
  {
    timer_next = timer->next;
    if( timer->type == TIMER_NUISANCE )
      extract_timer( victim, timer );
  }
  DISPOSE( victim->pcdata->nuisance );
  send_to_char( "Nuisance flag removed.\n\r", ch );
  return;
}

void do_pcrename( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char newname[MAX_STRING_LENGTH];
  char oldname[MAX_STRING_LENGTH];
  char backname[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  SHIP_DATA *ship_list;

  argument = one_argument( argument, arg1 );
  one_argument( argument, arg2 );
  smash_tilde( arg2 );


  if( IS_NPC( ch ) )
    return;

  if( arg1[0] == '\0' || arg2[0] == '\0' )
  {
    send_to_char( "Syntax: rename <victim> <new name>\n\r", ch );
    return;
  }

  if( !check_parse_name( arg2, 1 ) )
  {
    send_to_char( "Illegal name.\n\r", ch );
    return;
  }
  /*
   * Just a security precaution so you don't rename someone you don't mean
   * * too --Shaddai
   */
  if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    send_to_char( "That person is not in the room.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "You can't rename NPC's.\n\r", ch );
    return;
  }

  if( get_trust( ch ) < get_trust( victim ) )
  {
    send_to_char( "I don't think they would like that!\n\r", ch );
    return;
  }
  sprintf( newname, "%s%c/%s", PLAYER_DIR, tolower( arg2[0] ), capitalize( arg2 ) );
  sprintf( oldname, "%s%c/%s", PLAYER_DIR, tolower( victim->pcdata->filename[0] ), capitalize( victim->pcdata->filename ) );
  sprintf( backname, "%s%c/%s", BACKUP_DIR, tolower( victim->pcdata->filename[0] ), capitalize( victim->pcdata->filename ) );
  if( access( newname, F_OK ) == 0 )
  {
    send_to_char( "That name already exists.\n\r", ch );
    return;
  }

  /*
   * Have to remove the old god entry in the directories 
   */
  if( IS_IMMORTAL( victim ) )
  {
    char godname[MAX_STRING_LENGTH];
    sprintf( godname, "%s%s", GOD_DIR, capitalize( victim->pcdata->filename ) );
    remove( godname );
  }

  /*
   * Remember to change the names of the areas 
   */
  if( ch->pcdata->area )
  {
    char filename[MAX_STRING_LENGTH];
    char newfilename[MAX_STRING_LENGTH];

    sprintf( filename, "%s%s.are", BUILD_DIR, victim->name );
    sprintf( newfilename, "%s%s.are", BUILD_DIR, capitalize( arg2 ) );
    rename( filename, newfilename );
    sprintf( filename, "%s%s.are.bak", BUILD_DIR, victim->name );
    sprintf( newfilename, "%s%s.are.bak", BUILD_DIR, capitalize( arg2 ) );
    rename( filename, newfilename );
  }

  if( victim->pcdata->clan )
    remove_member( victim );

  STRFREE( victim->name );
  victim->name = STRALLOC( capitalize( arg2 ) );
  STRFREE( victim->pcdata->filename );
  victim->pcdata->filename = STRALLOC( capitalize( arg2 ) );
  remove( backname );
  if( remove( oldname ) )
  {
    sprintf( buf, "Error: Couldn't delete file %s in do_rename.", oldname );
    send_to_char( "Couldn't delete the old file!\n\r", ch );
    log_string( oldname );
  }
  /*
   * Time to save to force the affects to take place 
   */
  for( ship_list = first_ship; ship_list; ship_list = ship_list->next )
  {
    if( ship_list && ship_list->owner && !str_cmp( ship_list->owner, oldname ) )
    {
      STRFREE( ship_list->owner );
      ship_list->owner = STRALLOC( victim->name );
      save_ship( ship_list );
    }
  }
  if( victim->pcdata->clan )
    update_member( victim );
  save_char_obj( victim );

  /*
   * Now lets update the wizlist 
   */
  if( IS_IMMORTAL( victim ) )
    make_wizlist(  );
  send_to_char( "Character was renamed.\n\r", ch );
  return;
}

void do_pclastrename( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
//  char buf[MAX_STRING_LENGTH];

  argument = one_argument( argument, arg1 );
  one_argument( argument, arg2 );
  smash_tilde( arg2 );


  if( IS_NPC( ch ) )
    return;

  if( arg1[0] == '\0' )
  {
    send_to_char( "You must provide a victim name.\n\r", ch );
    return;
  }

  if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    send_to_char( "That person is not in the room.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_char( "You can't rename NPC's.\n\r", ch );
    return;
  }

  if( get_trust( ch ) < get_trust( victim ) )
  {
    send_to_char( "I don't think they would like that!\n\r", ch );
    return;
  }

  if( arg2[0] == '\0' )
  {
    if( victim->pcdata->last_name )
      DISPOSE( victim->pcdata->last_name );
    ch->pcdata->last_name = str_dup( " " );
    send_to_char( "Character's last name was emptied.\n\r", ch );
    return;
  }

  if( !check_parse_name( arg2, 1 ) )
  {
    send_to_char( "Illegal name.\n\r", ch );
    return;
  }
  else
  {
    char buf[512] = " ";
    strcpy( buf + 1, capitalize( arg2 ) );
    if( victim->pcdata->last_name )
      DISPOSE( victim->pcdata->last_name );
    victim->pcdata->last_name = str_dup( buf );
  }
  send_to_char( "Character's last name was changed.\n\r", ch );
  return;
}

int get_pc_race2( char *type )
{
  int i;

  for( i = 0; i < MAX_PC_RACE; i++ )
    if( !str_cmp( type, race_table[i]->race_name ) )
      return i;
  return -1;
}

/* Command to make everyone on the mud high all at once,
 * just for the fun of it. - Karma.
 */

void do_allhigh( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *vch;

  if( IS_NPC( ch ) )
    return;
  if( !IS_IMMORTAL( ch ) )
    return;

  if( argument[0] == '\0' )
  {
    ch_printf( ch, "\n\rSyntax: allhigh (on/off)\n\r" );
    return;
  }

  if( !str_cmp( argument, "on" ) )
  {
    for( vch = first_char; vch; vch = vch->next )
    {
      if( IS_NPC( vch ) )
        continue;
      if( !vch->desc )
        continue;
      vch->mental_state = 100;
      ch_printf( vch, "\n\r&RYou suddenly feel like you're tripping on acid!\n\r" );
    }
    ch_printf( ch, "&wAllhigh is now on.\n\r" );
    return;
  }
  if( !str_cmp( argument, "off" ) )
  {
    for( vch = first_char; vch; vch = vch->next )
    {
      if( IS_NPC( vch ) )
        continue;
      if( !vch->desc )
        continue;
      vch->mental_state = 0;
      ch_printf( vch, "\n\r&RYou're no longer high. Shit.\n\r" );
    }
    ch_printf( ch, "&wAllhigh is now off.\n\r" );
    return;
  }
}

/* GODSET command added by Warren
 * I'm tired of the limatations of do_mset
 * so I'm making my own set command.
 */
void do_godset( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int value = 0;
  long double valueld = 0;
  int minattr, maxattr;
  bool lockvictim;
  CLAN_DATA *clan = NULL;

  set_char_color( AT_PLAIN, ch );

  if( IS_NPC( ch ) )
  {
    send_to_char( "Mob's can't godset\n\r", ch );
    return;
  }

  if( !ch->desc )
  {
    send_to_char( "You have no descriptor\n\r", ch );
    return;
  }
  victim = NULL;
  lockvictim = FALSE;
  smash_tilde( argument );

  if( victim )
  {
    lockvictim = TRUE;
    strcpy( arg1, victim->name );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );
  }
  else
  {
    lockvictim = FALSE;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );
  }

  if( arg1[0] == '\0' || !str_cmp( arg1, "?" ) )
  {
    send_to_char( "Syntax: godset <victim> <field> <value>\n\r", ch );
    send_to_char( "\n\r", ch );
    send_to_char( "Field being one of:\n\r", ch );
    send_to_char( "  str int wis dex con cha luck sex level\n\r", ch );
    send_to_char( "  class race basepl currentpl\n\r", ch );
    send_to_char( "  lifeforce maxlife energy maxenergy\n\r", ch );
    send_to_char( "  prac train zeni align clan deity council\n\r", ch );
    send_to_char( "  thirst drunk hunger mental emotion xtrain\n\r", ch );
    send_to_char( "  name title password affected pretitle\n\r", ch );
    send_to_char( "  rage clrvnums aloaded silence \n\r", ch );
    send_to_char( "  pktimer pkyellow pkred pkills pdeaths\n\r", ch );
    send_to_char( "  mkills mdeaths illegal_pk sparwin sparloss \n\r", ch );
    send_to_char( "  bkills btimeleft naturalac lname sdcharge\n\r", ch );
    send_to_char( "  clanrank deathtimer\n\r", ch );
    if( ch->level >= 63 )
    {
      ch_printf( ch, "\n\rFusion stuff:\n\r" );
      ch_printf( ch, "  removefusion\n\r" );
    }
    if( ch->level >= 60 )
      ch_printf( ch, "\n\rOther stuff:\n\r" );
    ch_printf( ch, "  pwnt\n\r" );
    return;
  }

  if( !victim )
  {
    if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
      send_to_char( "No one like that in all the realms.\n\r", ch );
      return;
    }
  }

  minattr = 1;
  maxattr = 100;

  if( !str_cmp( arg2, "basepl" ) || !str_cmp( arg2, "currentpl" ) )
  {
    valueld = is_number( arg3 ) ? atof( arg3 ) : -1;
    if( atof( arg3 ) < -1 && valueld == -1 )
      valueld = atof( arg3 );
    pager_printf_color( ch, "&wGODSETING %s's %s to %0.Lf.\n\r", victim->name, arg2, valueld );
  }
  else
  {
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;
    if( atoi( arg3 ) < -1 && value == -1 )
      value = atoi( arg3 );
    if( is_number( arg3 ) )
      pager_printf_color( ch, "&wGODSETING %s's %s to %d.\n\r", victim->name, arg2, value );
    else
      pager_printf_color( ch, "&wGODSETING %s's %s to %s.\n\r", victim->name, arg2, arg3 );
  }

  if( !str_cmp( arg2, "removefusion" ) )
  {
    /*
     * if( !is_fused(victim) )
     * {
     * ch_printf(ch,"%s is not fused.\n\r",victim->name);
     * return;
     * }
     */
    int y = 0;
    int z = victim->fusions;
    for( y = 0; y < z; y++ )
    {
      STRFREE( victim->fused[y] );
    }
    victim->fusions = 0;
    victim->fusionflags = 0;
    if( is_android( victim ) )
    {
      victim->corespl = 0;
      victim->fm_core = FALSE;
      victim->e_core = FALSE;
      victim->h_core = FALSE;
    }
    ch_printf( ch, "Done.\n\r" );
    return;
  }

  if( !str_cmp( arg2, "pwnt" ) )
  {
    if( victim->level > ch->level )
    {
      send_to_char( "Can't pwnt someone over your own level.\n\r", ch );
      return;
    }
    SET_BIT( victim->fusionflags, FUSION_STASIS );
    ch_printf( ch, "Done.\n\r" );
    return;
  }
  /*
   * if (is_number(arg3))
   * {
   * value = is_number( arg3 ) ? atoi( arg3 ) : -1;
   * 
   * if ( atoi(arg3) < -1 && value == -1 )
   * value = atoi(arg3);
   * 
   * pager_printf_color (ch, "&zGODSETING %s's %s to %d.\n\r", victim->name, arg2, value);
   * }
   * else
   * pager_printf_color (ch, "&zGODSETING %s's %s to %d.\n\r", victim->name, arg2, arg3);
   */

  /*
   * Set PC's base powerlevel 
   */

  if( !str_cmp( arg2, "basepl" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( valueld < 1 )
    {
      ch_printf( ch, "Base powerlevel setting must be greater than 0.\n\r" );
      return;
    }
    if( valueld > pow( 10, 30 ) )
    {
      ch_printf( ch, "Base powerlevel setting can not be that high.\n\r" );
      victim->exp = pow( 10, 30 );
      return;
    }
    victim->exp = valueld;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's current powerlevel (mainly used to correct bugs) 
   */

  if( !str_cmp( arg2, "currentpl" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( valueld < 1 )
    {
      ch_printf( ch, "Current powerlevel setting must be greater than 0.\n\r" );
      return;
    }
    if( valueld > pow( 10, 30 ) )
    {
      ch_printf( ch, "Current powerlevel setting can not be that high.\n\r" );
      victim->pl = pow( 10, 30 );
      return;
    }
    victim->pl = valueld;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Toggles a builders area to 'loaded' 
   */

  if( !str_cmp( arg2, "aloaded" ) )
  {
    if( IS_NPC( victim ) )
    {
      send_to_char( "Player Characters only.\n\r", ch );
      return;
    }
    /*
     * Make sure they have an area assigned -Druid 
     */
    if( !victim->pcdata->area )
    {
      send_to_char( "Player does not have an area assigned to them.\n\r", ch );
      return;
    }


    if( !can_mmodify( ch, victim ) )
      return;

    if( !IS_SET( victim->pcdata->area->status, AREA_LOADED ) )
    {
      SET_BIT( victim->pcdata->area->status, AREA_LOADED );
      send_to_char( "Your area set to LOADED!\n\r", victim );
      if( ch != victim )
        send_to_char( "Area set to LOADED!\n\r", ch );
      return;
    }
    else
    {
      REMOVE_BIT( victim->pcdata->area->status, AREA_LOADED );
      send_to_char( "Your area set to NOT-LOADED!\n\r", victim );
      if( ch != victim )
        send_to_char( "Area set to NON-LOADED!\n\r", ch );
      return;
    }
  }

  /*
   * Clear A Builders Vnum Ranges 
   */

  if( !str_cmp( arg2, "clrvnums" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    victim->pcdata->r_range_lo = 0;
    victim->pcdata->r_range_hi = 0;
    victim->pcdata->o_range_lo = 0;
    victim->pcdata->o_range_hi = 0;
    victim->pcdata->m_range_lo = 0;
    victim->pcdata->m_range_hi = 0;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Change a PC's clan rank 
   */

  if( !str_cmp( arg2, "clanrank" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( !victim->pcdata->clan )
    {
      send_to_pager_color( "&wThat person doesn't belong to a clan.\n\r", ch );
      return;
    }
    clan = victim->pcdata->clan;
    if( victim->pcdata->clanRank == 0 )
      victim->pcdata->clanRank = value;
    else
    {
      switch ( victim->pcdata->clanRank )
      {
        default:
          break;
        case 7:
          if( victim->sex == SEX_FEMALE )
            clan->fRank7Count--;
          else
            clan->mRank7Count--;
          break;
        case 6:
          if( victim->sex == SEX_FEMALE )
            clan->fRank6Count--;
          else
            clan->mRank6Count--;
          break;
        case 5:
          if( victim->sex == SEX_FEMALE )
            clan->fRank5Count--;
          else
            clan->mRank5Count--;
          break;
        case 4:
          if( victim->sex == SEX_FEMALE )
            clan->fRank4Count--;
          else
            clan->mRank4Count--;
          break;
        case 3:
          if( victim->sex == SEX_FEMALE )
            clan->fRank3Count--;
          else
            clan->mRank3Count--;
          break;
        case 2:
          if( victim->sex == SEX_FEMALE )
            clan->fRank2Count--;
          else
            clan->mRank2Count--;
          break;
        case 1:
          if( victim->sex == SEX_FEMALE )
            clan->fRank1Count--;
          else
            clan->mRank1Count--;
          break;
      }
      victim->pcdata->clanRank = value;

      switch ( victim->pcdata->clanRank )
      {
        default:
          break;
        case 7:
          if( victim->sex == SEX_FEMALE )
            clan->fRank7Count++;
          else
            clan->mRank7Count++;
          break;
        case 6:
          if( victim->sex == SEX_FEMALE )
            clan->fRank6Count++;
          else
            clan->mRank6Count++;
          break;
        case 5:
          if( victim->sex == SEX_FEMALE )
            clan->fRank5Count++;
          else
            clan->mRank5Count++;
          break;
        case 4:
          if( victim->sex == SEX_FEMALE )
            clan->fRank4Count++;
          else
            clan->mRank4Count++;
          break;
        case 3:
          if( victim->sex == SEX_FEMALE )
            clan->fRank3Count++;
          else
            clan->mRank3Count++;
          break;
        case 2:
          if( victim->sex == SEX_FEMALE )
            clan->fRank2Count++;
          else
            clan->mRank2Count++;
          break;
        case 1:
          if( victim->sex == SEX_FEMALE )
            clan->fRank1Count++;
          else
            clan->mRank1Count++;
          break;
      }
    }
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Change a PC's self distruct charge number 
   */

  if( !str_cmp( arg2, "deathtimer" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    add_timer( victim, TIMER_PKILLED, value, NULL, 0 );
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "sdcharge" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    victim->pcdata->sd_charge = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Change a PC's last name 
   */

  if( !str_cmp( arg2, "lname" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    DISPOSE( victim->pcdata->last_name );
    victim->pcdata->last_name = str_dup( "" );
    buf2[0] = ' ';
    strcpy( buf2 + 1, argument );
    victim->pcdata->last_name = strdup( buf2 );
    /*
     * if( argument[0] == '\0' )
     * DISPOSE(victim->pcdata->last_name);
     */
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's Yellow PK flag 
   */

  if( !str_cmp( arg2, "pkyellow" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    if( !xIS_SET( victim->act, PLR_PK1 ) )
      xSET_BIT( victim->act, PLR_PK1 );

    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's Red PK flag 
   */

  if( !str_cmp( arg2, "pkred" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    if( !xIS_SET( victim->act, PLR_PK2 ) )
      xSET_BIT( victim->act, PLR_PK2 );

    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's b_timeleft 
   */

  if( !str_cmp( arg2, "btimeleft" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->b_timeleft = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }
  /*
   * Set PC's natural armor 
   */

  if( !str_cmp( arg2, "naturalac" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->natural_ac_max = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's pkills 
   */

  if( !str_cmp( arg2, "pkills" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->pkills = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's pdeaths 
   */

  if( !str_cmp( arg2, "pdeaths" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->pdeaths = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's mkills 
   */

  if( !str_cmp( arg2, "mkills" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->mkills = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's mdeaths 
   */

  if( !str_cmp( arg2, "mdeaths" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->mdeaths = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's illegal_pk 
   */

  if( !str_cmp( arg2, "illegal_pk" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->illegal_pk = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's sparwins 
   */

  if( !str_cmp( arg2, "sparwins" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->spar_wins = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's sparloss 
   */

  if( !str_cmp( arg2, "sparloss" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->spar_loss = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's bkills 
   */

  if( !str_cmp( arg2, "bkills" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->bkills = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's Silence timer 
   */

  if( !str_cmp( arg2, "silence" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->silence = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's PK timer 
   */

  if( !str_cmp( arg2, "pktimer" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->pk_timer = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's Rage 
   */

  if( !str_cmp( arg2, "rage" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->rage = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's base strength 
   */

  if( !str_cmp( arg2, "str" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < minattr || value > maxattr )
    {
      ch_printf( ch, "Strength range is %d to %d.\n\r", minattr, maxattr );
      return;
    }
    victim->perm_str = value;
    if( !IS_NPC( victim ) )
      victim->pcdata->permTstr = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's base intelligence 
   */

  if( !str_cmp( arg2, "int" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < minattr || value > maxattr )
    {
      ch_printf( ch, "Intelligence range is %d to %d.\n\r", minattr, maxattr );
      return;
    }
    victim->perm_int = value;
    if( !IS_NPC( victim ) )
      victim->pcdata->permTint = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's base dexterity 
   */

  if( !str_cmp( arg2, "dex" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < minattr || value > maxattr )
    {
      ch_printf( ch, "Dexterity range is %d to %d.\n\r", minattr, maxattr );
      return;
    }
    victim->perm_dex = value;
    if( !IS_NPC( victim ) )
      victim->pcdata->permTspd = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's base constitution 
   */

  if( !str_cmp( arg2, "con" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < minattr || value > maxattr )
    {
      ch_printf( ch, "Constitution range is %d to %d.\n\r", minattr, maxattr );
      return;
    }
    victim->perm_con = value;
    if( !IS_NPC( victim ) )
      victim->pcdata->permTcon = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's base luck 
   */

  if( !str_cmp( arg2, "luck" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < minattr || value > maxattr )
    {
      ch_printf( ch, "Luck range is %d to %d.\n\r", minattr, maxattr );
      return;
    }
    victim->perm_lck = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Change a PC's sex 
   */

  if( !str_cmp( arg2, "sex" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 0 || value > 2 )
    {
      send_to_char( "Sex range is 0 to 2.\n\r", ch );
      return;
    }
    victim->sex = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Change a PC's class 
   */

  if( !str_cmp( arg2, "class" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    if( value < 0 || value >= MAX_CLASS )
    {
      ch_printf( ch, "Class range is 0 to %d.\n", MAX_CLASS );
      return;
    }
    victim->class = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Change a PC's race 
   */

  if( !str_cmp( arg2, "race" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    value = get_pc_race2( arg3 );

    if( value < 0 )
      value = atoi( arg3 );
    if( !IS_NPC( victim ) && ( value < 0 || value >= MAX_RACE ) )
    {
      ch_printf( ch, "Race range is 0 to %d.\n", MAX_RACE - 1 );
      return;
    }

    victim->race = value;

    if(!is_saiyan(victim) && victim->pcdata && victim->pcdata->tail > -1) {
      victim->pcdata->tail = -1;
      send_to_char("Your tail shrinks away into nothingness as it is no more.", victim);
    }
      
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set a PC's level 
   */

  if( !str_cmp( arg2, "level" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    if( value < 0 || value > LEVEL_INFINITE )
    {
      ch_printf( ch, "Level range is 0 to %d.\n\r", LEVEL_INFINITE );
      return;
    }
    victim->level = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's gold 
   */

  if( !str_cmp( arg2, "zeni" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    victim->gold = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set a PC's current lifeforce 
   */

  if( !str_cmp( arg2, "lifeforce" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 1 || value > 32700 )
    {
      send_to_char( "Lifeforce range is 1 to 32,700 percent.\n\r", ch );
      return;
    }
    victim->hit = value;
    heart_calc( victim, "" );
    if( value != 100 )
    {
      send_to_char( "Rember, lifeforce is a percent of total health.\n\r", ch );
      sprintf( buf, "Setting %s's lifeforce to %d percent health.\n\r", arg1, value );
      send_to_char( buf, ch );
    }
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set a PC's max lifeforce 
   */

  if( !str_cmp( arg2, "maxlife" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 1 || value > 32700 )
    {
      send_to_char( "Lifeforce range is 1 to 32,700 percent.\n\r", ch );
      return;
    }
    victim->max_hit = value;
    if( value != 100 )
    {
      send_to_char( "Rember, lifeforce is a percent of total health.\n\r", ch );
      sprintf( buf, "Setting %s's lifeforce to %d percent health.\n\r", arg1, value );
      send_to_char( buf, ch );
    }
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }


  /*
   * Set a PC's current energy 
   */

  if( !str_cmp( arg2, "energy" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 0 || value > 99999 )
    {
      send_to_char( "Energy range is 0 to 99,999 points.\n\r", ch );
      return;
    }
    victim->mana = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set a PC's max energy 
   */

  if( !str_cmp( arg2, "maxenergy" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 0 || value > 99999 )
    {
      send_to_char( "Energy range is 0 to 99,999 points.\n\r", ch );
      return;
    }
    victim->max_mana = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set a PC's practice points 
   */

  if( !str_cmp( arg2, "prac" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 0 || value > 100 )
    {
      send_to_char( "Practice range is 0 to 100 sessions.\n\r", ch );
      return;
    }
    victim->practice = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set a PC's train points 
   */

  if( !str_cmp( arg2, "train" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 0 || value > 360 )
    {
      send_to_char( "Train range is 0 to 360 sessions.\n\r", ch );
      return;
    }
    victim->train = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set a PC's xtrain points 
   */

  if( !str_cmp( arg2, "xtrain" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    victim->pcdata->xTrain += value;
    victim->pcdata->total_xTrain += value;
    pager_printf( ch, "&wAdded %d xtrains to player.\n\r", value );
    return;
  }

  /*
   * Set a PC's alignment 
   */

  if( !str_cmp( arg2, "align" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < -1000 || value > 1000 )
    {
      send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
      return;
    }
    victim->alignment = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set a PC's password 
   */

  if( !str_cmp( arg2, "password" ) )
  {
    char *pwdnew;
    char *p;

    if( strlen( arg3 ) < 5 )
    {
      send_to_char( "New password must be at least five characters long.\n\r", ch );
      return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = smaug_crypt( arg3 );
    for( p = pwdnew; *p != '\0'; p++ )
    {
      if( *p == '~' )
      {
        send_to_char( "New password not acceptable, try again.\n\r", ch );
        return;
      }
    }

    DISPOSE( victim->pcdata->pwd );
    victim->pcdata->pwd = str_dup( pwdnew );
    if( IS_SET( sysdata.save_flags, SV_PASSCHG ) )
      save_char_obj( victim );
    send_to_char( "Ok.\n\r", ch );
    ch_printf( victim, "Your password has been changed by %s.\n\r", ch->name );
    return;
  }

  /*
   * Set a PC's mental state 
   */

  if( !str_cmp( arg2, "mental" ) )
  {
    if( value < -100 || value > 100 )
    {
      send_to_char( "Value must be in range -100 to +100.\n\r", ch );
      return;
    }
    victim->mental_state = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set a PC's emotional state 
   */

  if( !str_cmp( arg2, "emotion" ) )
  {
    if( value < -100 || value > 100 )
    {
      send_to_char( "Value must be in range -100 to +100.\n\r", ch );
      return;
    }
    victim->emotional_state = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set a PC's thirst value 
   */

  if( !str_cmp( arg2, "thirst" ) )
  {

    if( value < 0 || value > 100 )
    {
      send_to_char( "Thirst range is 0 to 100.\n\r", ch );
      return;
    }

    victim->pcdata->condition[COND_THIRST] = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set a PC's drunk level 
   */

  if( !str_cmp( arg2, "drunk" ) )
  {

    if( value < 0 || value > 100 )
    {
      send_to_char( "Drunk range is 0 to 100.\n\r", ch );
      return;
    }

    victim->pcdata->condition[COND_DRUNK] = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set a PC's hunger level 
   */

  if( !str_cmp( arg2, "full" ) )
  {

    if( value < 0 || value > 100 )
    {
      send_to_char( "Full range is 0 to 100.\n\r", ch );
      return;
    }

    victim->pcdata->condition[COND_FULL] = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Change a PC's name 
   */

  if( !str_cmp( arg2, "name" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    STRFREE( victim->name );
    victim->name = STRALLOC( arg3 );
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Change a PC's clan 
   */

  if( !str_cmp( arg2, "clan" ) )
  {
    CLAN_DATA *clan;

    if( !arg3 || arg3[0] == '\0' )
    {
      if( victim->pcdata->clan == NULL )
        return;
      if( !IS_IMMORTAL( victim ) )
      {
        --victim->pcdata->clan->members;
      }
      STRFREE( victim->pcdata->clan_name );
      victim->pcdata->clan_name = STRALLOC( "" );
      victim->pcdata->clan = NULL;
      update_member( victim );
      save_char_obj( victim );
      save_clan( victim->pcdata->clan );
      return;
    }
    clan = get_clan( arg3 );
    if( !clan )
    {
      send_to_char( "No such clan.\n\r", ch );
      return;
    }
    if( victim->pcdata->clan != NULL && !IS_IMMORTAL( victim ) )
    {
      --victim->pcdata->clan->members;
      save_clan( victim->pcdata->clan );
    }
    STRFREE( victim->pcdata->clan_name );
    victim->pcdata->clan_name = QUICKLINK( clan->name );
    victim->pcdata->clan = clan;
    if( !IS_IMMORTAL( victim ) )
    {
      ++victim->pcdata->clan->members;
    }
    update_member( victim );
    save_char_obj( victim );
    save_clan( victim->pcdata->clan );
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Change a PC's deity 
   */

  if( !str_cmp( arg2, "deity" ) )
  {
    DEITY_DATA *deity;

    if( !arg3 || arg3[0] == '\0' )
    {
      STRFREE( victim->pcdata->deity_name );
      victim->pcdata->deity_name = STRALLOC( "" );
      victim->pcdata->deity = NULL;
      send_to_char( "Deity removed.\n\r", ch );
      return;
    }

    deity = get_deity( arg3 );
    if( !deity )
    {
      send_to_char( "No such deity.\n\r", ch );
      return;
    }
    STRFREE( victim->pcdata->deity_name );
    victim->pcdata->deity_name = QUICKLINK( deity->name );
    victim->pcdata->deity = deity;
    send_to_char( "Done.\n\r", ch );
    return;
  }

  /*
   * Change a PC's council 
   */

  if( !str_cmp( arg2, "council" ) )
  {
    COUNCIL_DATA *council;

    if( !arg3 || arg3[0] == '\0' )
    {
      STRFREE( victim->pcdata->council_name );
      victim->pcdata->council_name = STRALLOC( "" );
      victim->pcdata->council = NULL;
      send_to_char( "Removed from council.\n\rPlease make sure you adjust that council's members accordingly.\n\r", ch );
      return;
    }

    council = get_council( arg3 );
    if( !council )
    {
      send_to_char( "No such council.\n\r", ch );
      return;
    }
    STRFREE( victim->pcdata->council_name );
    victim->pcdata->council_name = QUICKLINK( council->name );
    victim->pcdata->council = council;
    send_to_char( "Done.\n\rPlease make sure you adjust that council's members accordingly.\n\r", ch );
    return;
  }

  /*
   * Change a PC's title 
   */

  if( !str_cmp( arg2, "title" ) )
  {
    set_title( victim, arg3 );
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Change a PC's Pre-title 
   */

  if( !str_cmp( arg2, "pretitle" ) )
  {
    STRFREE( victim->pcdata->pretitle );
    victim->pcdata->pretitle = STRALLOC( arg3 );
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Change a PC's affected flags 
   */

  if( !str_cmp( arg2, "affected" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( !argument || argument[0] == '\0' )
    {
      send_to_char( "Usage: godset <victim> affected <flag> [flag]...\n\r", ch );
      return;
    }
    while( argument[0] != '\0' )
    {
      argument = one_argument( argument, arg3 );
      value = get_aflag( arg3 );
      if( value < 0 || value > MAX_BITS )
        ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
      else
        xTOGGLE_BIT( victim->affected_by, value );
    }
    return;
  }

  pager_printf_color( ch, "&wError: %s not found.", arg2 );
  return;
}

/* GODSTAT command added by Warren
 * I'm tired of the limatations of do_mstat
 * so I'm making my own stat command.
 */
void do_godstat( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  AFFECT_DATA *paf;

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_pager( "GODSTAT whom?\n\r", ch );
    return;
  }
  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_pager( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_NPC( victim ) )
  {
    send_to_pager( "Not on mobs.\n\r", ch );
    return;
  }

  set_pager_color( AT_SCORE, ch );

  pager_printf_color( ch, "\n\r&G%s%s%s.&C\n\r", victim->name,
                      victim->pcdata->last_name ? victim->pcdata->last_name : "", victim->pcdata->title );
  pager_printf( ch, "They are trusted at level %d.\n\r", get_trust( victim ) );
  send_to_pager( "----------------------------------------------------------------------------\n\r", ch );

  pager_printf_color( ch, "RACE : &W%-10.10s&C                           Played: &w%d&C hours\n\r",
                      capitalize( get_race( victim ) ), ( get_age( victim ) - 4 ) );

  pager_printf_color( ch, "YEARS: &W%-6d&C        SEX: &W%-7s&C           Log In: %s\r",
                      get_newage( victim ),
                      victim->sex == SEX_MALE ? "MALE" :
                      victim->sex == SEX_FEMALE ? "FEMALE" : "NEUTRAL", ctime( &( victim->logon ) ) );

  if( victim->level >= 15 || IS_PKILL( ch ) )
  {
    pager_printf_color( ch, "STR  : &W%2.2d&C(&w%2.2d&C)    HitRoll: &R%-4d&C              Saved:  %s\r",
                        get_curr_str( victim ), victim->perm_str, GET_HITROLL( victim ),
                        victim->save_time ? ctime( &( victim->save_time ) ) : "no save this session\n" );

    pager_printf_color( ch, "INT  : &W%2.2d&C(&w%2.2d&C)    DamRoll: &R%-4d&C              Time:   %s\r",
                        get_curr_int( victim ), victim->perm_int, GET_DAMROLL( victim ), ctime( &current_time ) );
  }
  else
  {
    pager_printf_color( ch, "STR  : &W%2.2d&C(&w%2.2d&C)                               Saved:  %s\r",
                        get_curr_str( victim ), victim->perm_str,
                        victim->save_time ? ctime( &( victim->save_time ) ) : "no\n" );

    pager_printf_color( ch, "INT  : %&W2.2d&C(&w%2.2d&C)                               Time:   %s\r",
                        get_curr_int( victim ), victim->perm_int, ctime( &current_time ) );
  }

  if( get_armor( ch ) < 0 )
    sprintf( buf, "&Wthe rags of a beggar&C" );
  else if( get_armor( ch ) == 0 )
    sprintf( buf, "&Wimproper for a battle&C" );
  else if( get_armor( ch ) < 500 )
    sprintf( buf, "&Wof poor quality&C" );
  else if( get_armor( ch ) < 2500 )
    sprintf( buf, "&Wmoderately crafted&C" );
  else if( get_armor( ch ) < 5000 )
    sprintf( buf, "&Wwell crafted&C" );
  else if( get_armor( ch ) < 7500 )
    sprintf( buf, "&Wexcellently crafted&C" );
  else if( get_armor( ch ) <= 10000 )
    sprintf( buf, "&Wexcellently crafted&C" );
  else
    sprintf( buf, "&WERROR: Please report&C" );
  pager_printf_color( ch, "WIS  : &W0&C(&w0&C)      Armor: &W%4.4d, &C%s\n\r", get_armor( victim ), buf );

  if( victim->alignment > 900 )
    sprintf( buf, "&Ydevout" );
  else if( victim->alignment > 700 )
    sprintf( buf, "&Ynoble" );
  else if( victim->alignment > 350 )
    sprintf( buf, "&Yhonorable" );
  else if( victim->alignment > 100 )
    sprintf( buf, "&Yworthy" );
  else if( victim->alignment > -100 )
    sprintf( buf, "&Wneutral" );
  else if( victim->alignment > -350 )
    sprintf( buf, "&Rbase" );
  else if( victim->alignment > -700 )
    sprintf( buf, "&Revil" );
  else if( victim->alignment > -900 )
    sprintf( buf, "&Rignoble" );
  else
    sprintf( buf, "&Rfiendish" );
  if( ch->level < LEVEL_AVATAR )
    pager_printf_color( ch, "DEX  : &W%2.2d&C(&w%2.2d&C)      Align: %-20.20s&C      Items: &W%3d&C  (&wmax %3d&C)\n\r",
                        get_curr_dex( victim ), victim->perm_dex, buf, victim->carry_number, can_carry_n( victim ) );
  else
    pager_printf_color( ch,
                        "DEX  : &W%2.2d&C(&w%2.2d&C)      Align: &W%+4.4d,&C %-14.14s&C     Items: &W%3d&C  (&wmax %3d&C)\n\r",
                        get_curr_dex( victim ), victim->perm_dex, victim->alignment, buf, victim->carry_number,
                        can_carry_n( victim ) );

  switch ( victim->position )
  {
    case POS_DEAD:
      sprintf( buf, "slowly decomposing" );
      break;
    case POS_MORTAL:
      sprintf( buf, "mortally wounded" );
      break;
    case POS_INCAP:
      sprintf( buf, "incapacitated" );
      break;
    case POS_STUNNED:
      sprintf( buf, "stunned" );
      break;
    case POS_SLEEPING:
      sprintf( buf, "sleeping" );
      break;
    case POS_RESTING:
      sprintf( buf, "resting" );
      break;
    case POS_STANDING:
      sprintf( buf, "standing" );
      break;
    case POS_FIGHTING:
      sprintf( buf, "fighting" );
      break;
    case POS_EVASIVE:
      sprintf( buf, "fighting (evasive)" ); /* Fighting style support -haus */
      break;
    case POS_DEFENSIVE:
      sprintf( buf, "fighting (defensive)" );
      break;
    case POS_AGGRESSIVE:
      sprintf( buf, "fighting (aggressive)" );
      break;
    case POS_BERSERK:
      sprintf( buf, "fighting (berserk)" );
      break;
    case POS_MOUNTED:
      sprintf( buf, "mounted" );
      break;
    case POS_SITTING:
      sprintf( buf, "sitting" );
      break;
  }
  pager_printf_color( ch, "CON  : &W%2.2d&C(&w%2.2d&C)      Pos'n: &W%-21.21s&C  Weight: &W%3d&C  (&wmax %3d&C)\n\r",
                      get_curr_con( victim ), victim->perm_con, buf, victim->carry_weight, can_carry_w( victim ) );


  /*
   * Fighting style support -haus
   */
  pager_printf_color( ch, "CHA  : &W0&C(&w0&C)      Wimpy: &R%-5d&C                   ", victim->wimpy );

  switch ( victim->style )
  {
    case STYLE_EVASIVE:
      sprintf( buf, "evasive" );
      break;
    case STYLE_DEFENSIVE:
      sprintf( buf, "defensive" );
      break;
    case STYLE_AGGRESSIVE:
      sprintf( buf, "aggressive" );
      break;
    case STYLE_BERSERK:
      sprintf( buf, "berserk" );
      break;
    default:
      sprintf( buf, "standard" );
      break;
  }
  pager_printf_color( ch, "Style: &W%-10.10s&C\n\r", buf );

  pager_printf_color( ch, "LCK  : &W%2.2d&C(&w%2.2d&C) \n\r", get_curr_lck( victim ), victim->perm_lck );

  pager_printf( ch, "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\r" );

  pager_printf_color( ch,
                      "PRACT: &G%2d&C/&g%2d&C    &YLifeforce&C: [&Y%3d&C]%c             AutoZeni: (&W%c&C)    AutoExit: (&W%c&C)\n\r",
                      victim->practice, victim->max_prac, victim->hit, '%', xIS_SET( victim->act, PLR_AUTOGOLD ) ? 'X' : ' ',
                      xIS_SET( victim->act, PLR_AUTOEXIT ) ? 'X' : ' ' );

  strcpy( buf, num_punct( victim->mana ) );
  strcpy( buf2, num_punct( victim->max_mana ) );

  if( victim->max_mana < 1000 )
    pager_printf_color( ch,
                        "TRAIN: &G%2d&C/&g%2d&C       &YEnergy&C: [&Y%3s&C/&Y%-3s&C]           Sparing: (&W%c&C)    AutoLoot: (&W%c&C)\n\r",
                        victim->train, victim->max_train, buf, buf2, xIS_SET( victim->act, PLR_SPAR ) ? 'X' : ' ',
                        xIS_SET( victim->act, PLR_AUTOLOOT ) ? 'X' : ' ' );
  else if( victim->max_mana < 10000 )
    pager_printf_color( ch,
                        "TRAIN: &G%2d&C/&g%2d&C       &YEnergy&C: [&Y%5s&C/&Y%-5s&C]       Sparing: (&W%c&C)    AutoLoot: (&W%c&C)\n\r",
                        victim->train, victim->max_train, buf, buf2, xIS_SET( victim->act, PLR_SPAR ) ? 'X' : ' ',
                        xIS_SET( victim->act, PLR_AUTOLOOT ) ? 'X' : ' ' );
  else
    pager_printf_color( ch,
                        "TRAIN: &G%2d&C/&g%2d&C       &YEnergy&C: [&Y%6s&C/&Y%-6s&C]     Sparing: (&W%c&C)    AutoLoot: (&W%c&C)\n\r",
                        victim->train, victim->max_train, buf, buf2, xIS_SET( victim->act, PLR_SPAR ) ? 'X' : ' ',
                        xIS_SET( victim->act, PLR_AUTOLOOT ) ? 'X' : ' ' );

  pager_printf_color( ch, "ZENI : &Y%-13s&C                                            AutoSac: (&W%c&C)\n\r",
                      num_punct( victim->gold ), xIS_SET( victim->act, PLR_AUTOSAC ) ? 'X' : ' ' );

  pager_printf( ch, "BASE POWERLEVEL: %-16s\n\r", num_punct_ld( victim->exp ) );

  if( victim->pl != victim->exp )
  {
    set_pager_color( AT_YELLOW, ch );
    pager_printf( ch, "CURR POWERLEVEL: %-16s\n\r", num_punct_ld( victim->pl ) );
    set_pager_color( AT_SCORE, ch );
  }
  else
    pager_printf( ch, "CURR POWERLEVEL: %-16s\n\r", num_punct_ld( victim->pl ) );

  pager_printf_color( ch, "                         PKills: [&R%5.5d&C]  IllegalPKs: [&R%3.3d&C]  MKills: [&R%5.5d&C]\n\r",
                      victim->pcdata->pkills, victim->pcdata->illegal_pk, victim->pcdata->mkills );
  pager_printf_color( ch, "                       SparWins: [&R%5.5d&C]     PDeaths: [&R%3.3d&C] MDeaths: [&R%5.5d&C]\n\r",
                      victim->pcdata->spar_wins, victim->pcdata->pdeaths, victim->pcdata->mdeaths );
  pager_printf_color( ch, "                       SparLoss: [&R%5.5d&C]      Self Destruct Charges: [&R%5.5d&C]\n\r",
                      victim->pcdata->spar_loss, victim->pcdata->sd_charge );

  if( !IS_NPC( ch ) && victim->pcdata->condition[COND_DRUNK] > 10 )
    send_to_pager( "You are drunk.\n\r", ch );
  if( !IS_NPC( ch ) && victim->pcdata->condition[COND_THIRST] == 0 )
    send_to_pager( "You are in danger of dehydrating.\n\r", ch );
  if( !IS_NPC( ch ) && victim->pcdata->condition[COND_FULL] == 0 )
    send_to_pager( "You are starving to death.\n\r", ch );
  if( victim->position != POS_SLEEPING )
    switch ( victim->mental_state / 10 )
    {
      default:
        send_to_pager( "They're completely messed up!\n\r", ch );
        break;
      case -10:
        send_to_pager( "They're barely conscious.\n\r", ch );
        break;
      case -9:
        send_to_pager( "They can barely keep your eyes open.\n\r", ch );
        break;
      case -8:
        send_to_pager( "They're extremely drowsy.\n\r", ch );
        break;
      case -7:
        send_to_pager( "They feel very unmotivated.\n\r", ch );
        break;
      case -6:
        send_to_pager( "They feel sedated.\n\r", ch );
        break;
      case -5:
        send_to_pager( "They feel sleepy.\n\r", ch );
        break;
      case -4:
        send_to_pager( "They feel tired.\n\r", ch );
        break;
      case -3:
        send_to_pager( "They could use a rest.\n\r", ch );
        break;
      case -2:
        send_to_pager( "They feel a little under the weather.\n\r", ch );
        break;
      case -1:
        send_to_pager( "They feel fine.\n\r", ch );
        break;
      case 0:
        send_to_pager( "They feel great.\n\r", ch );
        break;
      case 1:
        send_to_pager( "They feel energetic.\n\r", ch );
        break;
      case 2:
        send_to_pager( "Their mind is racing.\n\r", ch );
        break;
      case 3:
        send_to_pager( "They can't think straight.\n\r", ch );
        break;
      case 4:
        send_to_pager( "Their mind is going 100 miles an hour.\n\r", ch );
        break;
      case 5:
        send_to_pager( "They're high as a kite.\n\r", ch );
        break;
      case 6:
        send_to_pager( "Their mind and body are slipping apart.\n\r", ch );
        break;
      case 7:
        send_to_pager( "Their reality is slipping away.\n\r", ch );
        break;
      case 8:
        send_to_pager( "They have no idea what is real, and what is not.\n\r", ch );
        break;
      case 9:
        send_to_pager( "They feel immortal.\n\r", ch );
        break;
      case 10:
        send_to_pager( "They are a Supreme Entity.\n\r", ch );
        break;
    }
  else if( victim->mental_state > 45 )
    send_to_pager( "Their sleep is filled with strange and vivid dreams.\n\r", ch );
  else if( victim->mental_state > 25 )
    send_to_pager( "Their sleep is uneasy.\n\r", ch );
  else if( victim->mental_state < -35 )
    send_to_pager( "They are deep in a much needed sleep.\n\r", ch );
  else if( victim->mental_state < -25 )
    send_to_pager( "They are in deep slumber.\n\r", ch );
  send_to_pager( "----------------------------------------------------------------------------\n\r", ch );
  pager_printf_color( ch, "&CAffected by: &W%s&C\n\r", affect_bit_name( &victim->affected_by ) );

  if( victim->pcdata->bestowments && victim->pcdata->bestowments[0] != '\0' )
    pager_printf( ch, "You are bestowed with the command(s): %s.\n\r", victim->pcdata->bestowments );

  if( victim->morph && victim->morph->morph )
  {
    send_to_pager( "----------------------------------------------------------------------------\n\r", ch );
    if( IS_IMMORTAL( victim ) )
      pager_printf( ch, "Morphed as (%d) %s with a timer of %d.\n\r",
                    victim->morph->morph->vnum, victim->morph->morph->short_desc, victim->morph->timer );
    else
      pager_printf( ch, "You are morphed into a %s.\n\r", victim->morph->morph->short_desc );
    send_to_pager( "----------------------------------------------------------------------------\n\r", ch );
  }
  if( victim->pcdata->clan && victim->pcdata->clan->clan_type != CLAN_ORDER
      && victim->pcdata->clan->clan_type != CLAN_GUILD )
  {
    send_to_pager( "----------------------------------------------------------------------------\n\r", ch );
    pager_printf( ch, "CLAN STATS:  %-14.14s  Clan AvPkills : %-5d  Clan NonAvpkills : %-5d\n\r",
                  victim->pcdata->clan->name, victim->pcdata->clan->pkills[6],
                  ( victim->pcdata->clan->pkills[1] + victim->pcdata->clan->pkills[2] +
                    victim->pcdata->clan->pkills[3] + victim->pcdata->clan->pkills[4] + victim->pcdata->clan->pkills[5] ) );
    pager_printf( ch, "                             Clan AvPdeaths: %-5d  Clan NonAvpdeaths: %-5d\n\r",
                  victim->pcdata->clan->pdeaths[6],
                  ( victim->pcdata->clan->pdeaths[1] + victim->pcdata->clan->pdeaths[2] +
                    victim->pcdata->clan->pdeaths[3] + victim->pcdata->clan->pdeaths[4] +
                    victim->pcdata->clan->pdeaths[5] ) );
  }
  if( victim->pcdata->deity )
  {
    send_to_pager( "----------------------------------------------------------------------------\n\r", ch );
    if( victim->pcdata->favor > 2250 )
      sprintf( buf, "loved" );
    else if( victim->pcdata->favor > 2000 )
      sprintf( buf, "cherished" );
    else if( victim->pcdata->favor > 1750 )
      sprintf( buf, "honored" );
    else if( victim->pcdata->favor > 1500 )
      sprintf( buf, "praised" );
    else if( victim->pcdata->favor > 1250 )
      sprintf( buf, "favored" );
    else if( victim->pcdata->favor > 1000 )
      sprintf( buf, "respected" );
    else if( victim->pcdata->favor > 750 )
      sprintf( buf, "liked" );
    else if( victim->pcdata->favor > 250 )
      sprintf( buf, "tolerated" );
    else if( victim->pcdata->favor > -250 )
      sprintf( buf, "ignored" );
    else if( victim->pcdata->favor > -750 )
      sprintf( buf, "shunned" );
    else if( victim->pcdata->favor > -1000 )
      sprintf( buf, "disliked" );
    else if( victim->pcdata->favor > -1250 )
      sprintf( buf, "dishonored" );
    else if( victim->pcdata->favor > -1500 )
      sprintf( buf, "disowned" );
    else if( victim->pcdata->favor > -1750 )
      sprintf( buf, "abandoned" );
    else if( victim->pcdata->favor > -2000 )
      sprintf( buf, "despised" );
    else if( victim->pcdata->favor > -2250 )
      sprintf( buf, "hated" );
    else
      sprintf( buf, "damned" );
    pager_printf( ch, "Deity:  %-20s  Favor: %s\n\r", ch->pcdata->deity->name, buf );
  }
  if( victim->pcdata->clan && victim->pcdata->clan->clan_type == CLAN_ORDER )
  {
    send_to_pager( "----------------------------------------------------------------------------\n\r", ch );
    pager_printf( ch, "Order:  %-20s  Order Mkills:  %-6d   Order MDeaths:  %-6d\n\r",
                  victim->pcdata->clan->name, victim->pcdata->clan->mkills, victim->pcdata->clan->mdeaths );
  }
  if( victim->pcdata->clan && victim->pcdata->clan->clan_type == CLAN_GUILD )
  {
    send_to_pager( "----------------------------------------------------------------------------\n\r", ch );
    pager_printf( ch, "Guild:  %-20s  Guild Mkills:  %-6d   Guild MDeaths:  %-6d\n\r",
                  victim->pcdata->clan->name, victim->pcdata->clan->mkills, victim->pcdata->clan->mdeaths );
  }
  if( IS_IMMORTAL( victim ) )
  {
    send_to_pager( "----------------------------------------------------------------------------\n\r", ch );

    pager_printf( ch, "IMMORTAL DATA:  Wizinvis [%s]  Wizlevel (%d)\n\r",
                  xIS_SET( victim->act, PLR_WIZINVIS ) ? "X" : " ", victim->pcdata->wizinvis );

    pager_printf( ch, "Bamfin:  %s %s\n\r", victim->name, ( victim->pcdata->bamfin[0] != '\0' )
                  ? victim->pcdata->bamfin : "appears in a swirling mist." );
    pager_printf( ch, "Bamfout: %s %s\n\r", victim->name, ( victim->pcdata->bamfout[0] != '\0' )
                  ? victim->pcdata->bamfout : "leaves in a swirling mist." );


    if( victim->pcdata->area )
    {
      pager_printf( ch, "Vnums:   Room (%-5.5d - %-5.5d)   Object (%-5.5d - %-5.5d)   Mob (%-5.5d - %-5.5d)\n\r",
                    victim->pcdata->area->low_r_vnum, victim->pcdata->area->hi_r_vnum,
                    victim->pcdata->area->low_o_vnum, victim->pcdata->area->hi_o_vnum,
                    victim->pcdata->area->low_m_vnum, victim->pcdata->area->hi_m_vnum );
      pager_printf( ch, "Area Loaded [%s]\n\r", ( IS_SET( victim->pcdata->area->status, AREA_LOADED ) ) ? "yes" : "no" );
    }
  }
  if( victim->first_affect )
  {
    int i;
    SKILLTYPE *sktmp;

    i = 0;
    send_to_pager( "----------------------------------------------------------------------------\n\r", ch );
    send_to_pager( "AFFECT DATA:                            ", ch );
    for( paf = victim->first_affect; paf; paf = paf->next )
    {
      if( ( sktmp = get_skilltype( paf->type ) ) == NULL )
        continue;
      if( ch->level < 20 )
      {
        pager_printf( ch, "[%-34.34s]    ", sktmp->name );
        if( i == 0 )
          i = 2;
        if( ( ++i % 3 ) == 0 )
          send_to_pager( "\n\r", ch );
      }
      if( ch->level >= 20 )
      {
        if( paf->modifier == 0 )
          pager_printf( ch, "[%-24.24s;%5d rds]    ", sktmp->name, paf->duration );
        else if( paf->modifier > 999 )
          pager_printf( ch, "[%-15.15s; %7.7s;%5d rds]    ",
                        sktmp->name, tiny_affect_loc_name( paf->location ), paf->duration );
        else
          pager_printf( ch, "[%-11.11s;%+-3.3d %7.7s;%5d rds]    ",
                        sktmp->name, paf->modifier, tiny_affect_loc_name( paf->location ), paf->duration );
        if( i == 0 )
          i = 1;
        if( ( ++i % 2 ) == 0 )
          send_to_pager( "\n\r", ch );
      }
    }
  }

  pager_printf( ch, "Silence time left: %d   Safe from bounty time left:      %d\n\r",
                victim->pcdata->silence, victim->pcdata->b_timeleft );
  pager_printf( ch, "PK time left:      %d\n\r", victim->pcdata->pk_timer );
  pager_printf( ch, "Creation Date:     %s\n\r", ( char * )ctime( &victim->pcdata->creation_date ) );
  send_to_pager( "&w&D\n\r", ch );
  return;
}

void do_godwho( CHAR_DATA * ch, char *argument )
{
  DESCRIPTOR_DATA *d;
  sh_int cswitch = 1;
  CHAR_DATA *vch;
  sh_int sex = 0;

  if( !str_cmp( argument, "m" ) )
    sex = SEX_MALE;
  else if( !str_cmp( argument, "f" ) )
    sex = SEX_FEMALE;
  else if( !str_cmp( argument, "n" ) )
    sex = SEX_NEUTRAL;
  else if( !str_cmp( argument, "a" ) )
    sex = -1;
  else
  {
    pager_printf_color( ch, "&wSyntax: godwho m/f/n/a" );
    return;
  }

  send_to_pager_color( "\n\r&B=----------------------------------------------------------------------------=\n\r", ch );

  for( d = first_descriptor; d; d = d->next )
  {
    vch = d->character;

    if( !vch )
    {
      continue;
    }

    if( vch->sex == sex || sex == -1 )
    {
      if( cswitch == 1 )
      {
        pager_printf_color( ch, "&B|&W[%10s][HOURS][%3d] Base/Current PL:[%15s]/",
                            vch->name, ( get_age( vch ) - 4 ), num_punct_ld( vch->exp ) );
        pager_printf_color( ch, "&W[%15s]Energy:[%6s]&B|\n\r", num_punct_ld( vch->pl ), num_punct( vch->max_mana ) );
        cswitch = 2;
      }
      else
      {
        pager_printf_color( ch, "&B|&Y[%10s][HOURS][%3d] Base/Current PL:[%15s]/",
                            vch->name, ( get_age( vch ) - 4 ), num_punct_ld( vch->exp ) );
        pager_printf_color( ch, "&Y[%15s]Energy:[%6s]&B|\n\r", num_punct_ld( vch->pl ), num_punct( vch->max_mana ) );
        cswitch = 1;
      }
    }

  }
  send_to_pager_color( "&B=----------------------------------------------------------------------------=\n\r", ch );

  return;
}

/* Ghost hides the immortal in the room they are standing.
 * They still show up on who.   -Froboz was here :)
 */
void do_ghost( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  sh_int level;

  if( IS_NPC( ch ) )
    return;

  argument = one_argument( argument, arg );
  if( arg && arg[0] != '\0' )
  {
    if( !is_number( arg ) )
    {
      send_to_char( "Usage: ghost | ghost <level>\n\r", ch );
      return;
    }
    level = atoi( arg );
    if( level < 2 || level > get_trust( ch ) )
    {
      send_to_char( "Invalid level.\n\r", ch );
      return;
    }

    ch->pcdata->ghost_level = level;
    ch_printf( ch, "Ghost level set to %d.\n\r", level );

    return;
  }

  if( !IS_NPC( ch ) )
  {
    if( ch->pcdata->ghost_level < 2 )
      ch->pcdata->ghost_level = ch->level;
  }
  if( xIS_SET( ch->act, PLR_GHOST ) )
  {
    xREMOVE_BIT( ch->act, PLR_GHOST );
    act( AT_WHITE, "$n steps out from the mist.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You step out from the mist.\n\r", ch );
  }
  else
  {
    act( AT_WHITE, "$n vanishes into a mist.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You vanish into a mist.\n\r", ch );
    xSET_BIT( ch->act, PLR_GHOST );
  }
  return;
}

/* Incognito is a level based command. Once cloaked,
 * all players up to the level set will not be able
 * to see you, unless they are in the same room.
 * -Froboz
 */
void do_incognito( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  sh_int level;

  if( IS_NPC( ch ) )
    return;

  argument = one_argument( argument, arg );
  if( arg && arg[0] != '\0' )
  {
    if( !is_number( arg ) )
    {
      send_to_char( "Usage: incognito | incognito <level>\n\r", ch );
      return;
    }
    level = atoi( arg );
    if( level < 2 || level > get_trust( ch ) )
    {
      send_to_char( "Invalid level.\n\r", ch );
      return;
    }

    ch->pcdata->incog_level = level;
    ch_printf( ch, "Incognito level set to %d.\n\r", level );

    return;
  }

  if( !IS_NPC( ch ) )
  {
    if( ch->pcdata->incog_level < 2 )
      ch->pcdata->incog_level = ch->level;
  }
  if( xIS_SET( ch->act, PLR_INCOG ) )
  {
    xREMOVE_BIT( ch->act, PLR_INCOG );
    act( AT_BLUE, "$n is no longer cloaked.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You are no longer cloaked.\n\r", ch );
  }
  else
  {
    act( AT_BLUE, "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You cloak your presence.\n\r", ch );
    xSET_BIT( ch->act, PLR_INCOG );
  }
  return;
}

/* REIMBURSE command added by Warren
 * For admins that need to do this kind
 * of stuff
 */
void do_reimburse( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int value = 0;
  long double valueld = 0;
  int minattr, maxattr;
  bool lockvictim;

  set_char_color( AT_PLAIN, ch );

  if( IS_NPC( ch ) )
  {
    send_to_char( "Mob's can't godset\n\r", ch );
    return;
  }

  if( !ch->desc )
  {
    send_to_char( "You have no descriptor\n\r", ch );
    return;
  }

  switch ( ch->substate )
  {
    default:
      break;
    case SUB_MOB_DESC:
      if( !ch->dest_buf )
      {
        send_to_char( "Fatal error: report to Goku.\n\r", ch );
        bug( "do_mset: sub_mob_desc: NULL ch->dest_buf", 0 );
        ch->substate = SUB_NONE;
        return;
      }
      victim = ch->dest_buf;
      if( char_died( victim ) )
      {
        send_to_char( "Your victim died!\n\r", ch );
        stop_editing( ch );
        return;
      }
      STRFREE( victim->description );
      victim->description = copy_buffer( ch );
      if( IS_NPC( victim ) && xIS_SET( victim->act, ACT_PROTOTYPE ) )
      {
        STRFREE( victim->pIndexData->description );
        victim->pIndexData->description = QUICKLINK( victim->description );
      }
      stop_editing( ch );
      ch->substate = ch->tempnum;
      return;
  }

  victim = NULL;
  lockvictim = FALSE;
  smash_tilde( argument );

  if( ch->substate == SUB_REPEATCMD )
  {
    victim = ch->dest_buf;

    if( char_died( victim ) )
    {
      send_to_char( "Your victim died!\n\r", ch );
      victim = NULL;
      argument = "done";
    }
    if( argument[0] == '\0' || !str_cmp( argument, " " ) || !str_cmp( argument, "stat" ) )
    {
      if( victim )
        do_mstat( ch, victim->name );
      else
        send_to_char( "No victim selected.  Type '?' for help.\n\r", ch );
      return;
    }
    if( !str_cmp( argument, "done" ) || !str_cmp( argument, "off" ) )
    {
      send_to_char( "Mset mode off.\n\r", ch );
      ch->substate = SUB_NONE;
      ch->dest_buf = NULL;
      if( ch->pcdata && ch->pcdata->subprompt )
      {
        STRFREE( ch->pcdata->subprompt );
        ch->pcdata->subprompt = NULL;
      }
      return;
    }
  }
  if( victim )
  {
    lockvictim = TRUE;
    strcpy( arg1, victim->name );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );
  }
  else
  {
    lockvictim = FALSE;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );
  }

  if( arg1[0] == '\0' || !str_cmp( arg1, "?" ) )
  {
    send_to_char( "Syntax: reimburse <victim> <field> <value>\n\r", ch );
    send_to_char( "\n\r", ch );
    send_to_char( "Field being one of:\n\r", ch );
    send_to_char( "  str int wis dex con cha luck sex\n\r", ch );
    send_to_char( "  basepl currentpl lifeforce maxlife\n\r", ch );
    send_to_char( "  energy maxenergy prac train zeni \n\r", ch );
    send_to_char( "  align clan deity council thirst\n\r", ch );
    send_to_char( "  drunk hunger mental emotion\n\r", ch );
    send_to_char( "  password title pretitle\n\r", ch );
    send_to_char( "  clrvnums aloaded silence pktimer\n\r", ch );
    send_to_char( "  pkyellow pkred \n\r", ch );

    return;
  }

  if( !victim && get_trust( ch ) < LEVEL_SUPREME )
  {
    if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }
  }
  else if( !victim )
  {
    if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
      send_to_char( "No one like that in all the realms.\n\r", ch );
      return;
    }
  }

  minattr = 1;
  maxattr = 100;

  if( !str_cmp( arg2, "basepl" ) || !str_cmp( arg2, "currentpl" ) )
  {
    valueld = is_number( arg3 ) ? atof( arg3 ) : -1;
    if( atof( arg3 ) < -1 && valueld == -1 )
      valueld = atof( arg3 );
    pager_printf_color( ch, "&wReimbursing %s's %s to %0.Lf.\n\r", victim->name, arg2, valueld );
  }
  else
  {
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;
    if( atoi( arg3 ) < -1 && value == -1 )
      value = atoi( arg3 );
    if( is_number( arg3 ) )
      pager_printf_color( ch, "&wReimbursing %s's %s to %d.\n\r", victim->name, arg2, value );
    else
      pager_printf_color( ch, "&wReimbursing %s's %s to %s.\n\r", victim->name, arg2, arg3 );

  }

  /*
   * if (is_number(arg3))
   * {
   * value = is_number( arg3 ) ? atoi( arg3 ) : -1;
   * 
   * if ( atoi(arg3) < -1 && value == -1 )
   * value = atoi(arg3);
   * 
   * pager_printf_color (ch, "&zGODSETING %s's %s to %d.\n\r", victim->name, arg2, value);
   * }
   * else
   * pager_printf_color (ch, "&zGODSETING %s's %s to %d.\n\r", victim->name, arg2, arg3);
   */

  /*
   * Set PC's base powerlevel 
   */

  if( !str_cmp( arg2, "basepl" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( valueld < 1 )
    {
      ch_printf( ch, "Base powerlevel setting must be greater than 0.\n\r" );
      return;
    }
    if( valueld > pow( 10, 30 ) )
    {
      ch_printf( ch, "Base powerlevel setting can not be that high.\n\r" );
      victim->exp = pow( 10, 30 );
      return;
    }
    victim->exp = valueld;
    send_to_pager_color( "&wOk.", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set PC's current powerlevel (mainly used to correct bugs) 
   */

  if( !str_cmp( arg2, "currentpl" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( valueld < 1 )
    {
      ch_printf( ch, "Current powerlevel setting must be greater than 0.\n\r" );
      return;
    }
    if( valueld > pow( 10, 30 ) )
    {
      ch_printf( ch, "Current powerlevel setting can not be that high.\n\r" );
      victim->pl = pow( 10, 30 );
      return;
    }
    victim->pl = valueld;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Toggles a builders area to 'loaded' 
   */

  if( !str_cmp( arg2, "aloaded" ) )
  {
    if( IS_NPC( victim ) )
    {
      send_to_char( "Player Characters only.\n\r", ch );
      return;
    }
    /*
     * Make sure they have an area assigned -Druid 
     */
    if( !victim->pcdata->area )
    {
      send_to_char( "Player does not have an area assigned to them.\n\r", ch );
      return;
    }


    if( !can_mmodify( ch, victim ) )
      return;

    if( !IS_SET( victim->pcdata->area->status, AREA_LOADED ) )
    {
      SET_BIT( victim->pcdata->area->status, AREA_LOADED );
      send_to_char( "Your area set to LOADED!\n\r", victim );
      if( ch != victim )
        send_to_char( "Area set to LOADED!\n\r", ch );
      return;
    }
    else
    {
      REMOVE_BIT( victim->pcdata->area->status, AREA_LOADED );
      send_to_char( "Your area set to NOT-LOADED!\n\r", victim );
      if( ch != victim )
        send_to_char( "Area set to NON-LOADED!\n\r", ch );
      return;
    }
  }

  /*
   * Clear A Builders Vnum Ranges 
   */

  if( !str_cmp( arg2, "clrvnums" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    victim->pcdata->r_range_lo = 0;
    victim->pcdata->r_range_hi = 0;
    victim->pcdata->o_range_lo = 0;
    victim->pcdata->o_range_hi = 0;
    victim->pcdata->m_range_lo = 0;
    victim->pcdata->m_range_hi = 0;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&w%s has cleared your vnum range.\n\r", ch->name );
    return;
  }

  /*
   * Set PC's Yellow PK flag 
   */

  if( !str_cmp( arg2, "pkyellow" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    if( !xIS_SET( victim->act, PLR_PK1 ) )
      xSET_BIT( victim->act, PLR_PK1 );

    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's Red PK flag 
   */

  if( !str_cmp( arg2, "pkred" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    if( !xIS_SET( victim->act, PLR_PK2 ) )
      xSET_BIT( victim->act, PLR_PK2 );

    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  /*
   * Set PC's Silence timer 
   */

  if( !str_cmp( arg2, "silence" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->silence = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set PC's PK timer 
   */

  if( !str_cmp( arg2, "pktimer" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->pk_timer = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set PC's base strength 
   */

  if( !str_cmp( arg2, "str" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < minattr || value > maxattr )
    {
      ch_printf( ch, "Strength range is %d to %d.\n\r", minattr, maxattr );
      return;
    }
    victim->perm_str = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set PC's base intelligence 
   */

  if( !str_cmp( arg2, "int" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < minattr || value > maxattr )
    {
      ch_printf( ch, "Intelligence range is %d to %d.\n\r", minattr, maxattr );
      return;
    }
    victim->perm_int = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set PC's base dexterity 
   */

  if( !str_cmp( arg2, "dex" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < minattr || value > maxattr )
    {
      ch_printf( ch, "Dexterity range is %d to %d.\n\r", minattr, maxattr );
      return;
    }
    victim->perm_dex = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set PC's base constitution 
   */

  if( !str_cmp( arg2, "con" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < minattr || value > maxattr )
    {
      ch_printf( ch, "Constitution range is %d to %d.\n\r", minattr, maxattr );
      return;
    }
    victim->perm_con = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set PC's base luck 
   */

  if( !str_cmp( arg2, "luck" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < minattr || value > maxattr )
    {
      ch_printf( ch, "Luck range is %d to %d.\n\r", minattr, maxattr );
      return;
    }
    victim->perm_lck = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Change a PC's sex 
   */

  if( !str_cmp( arg2, "sex" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 0 || value > 2 )
    {
      send_to_char( "Sex range is 0 to 2.\n\r", ch );
      return;
    }
    victim->sex = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set PC's gold 
   */

  if( !str_cmp( arg2, "zeni" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    victim->gold = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set a PC's current lifeforce 
   */

  if( !str_cmp( arg2, "lifeforce" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 1 || value > 32700 )
    {
      send_to_char( "Lifeforce range is 1 to 32,700 percent.\n\r", ch );
      return;
    }
    victim->hit = value;
    heart_calc( victim, "" );
    if( value != 100 )
    {
      send_to_char( "Rember, lifeforce is a percent of total health.\n\r", ch );
      sprintf( buf, "Setting %s's lifeforce to %d percent health.\n\r", arg1, value );
      send_to_char( buf, ch );
    }
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set a PC's max lifeforce 
   */

  if( !str_cmp( arg2, "maxlife" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 1 || value > 32700 )
    {
      send_to_char( "Lifeforce range is 1 to 32,700 percent.\n\r", ch );
      return;
    }
    victim->max_hit = value;
    if( value != 100 )
    {
      send_to_char( "Rember, lifeforce is a percent of total health.\n\r", ch );
      sprintf( buf, "Setting %s's lifeforce to %d percent health.\n\r", arg1, value );
      send_to_char( buf, ch );
    }
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }


  /*
   * Set a PC's current energy 
   */

  if( !str_cmp( arg2, "energy" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 0 || value > 99999 )
    {
      send_to_char( "Energy range is 0 to 99,999 points.\n\r", ch );
      return;
    }
    victim->mana = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set a PC's max energy 
   */

  if( !str_cmp( arg2, "maxenergy" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 0 || value > 99999 )
    {
      send_to_char( "Energy range is 0 to 99,999 points.\n\r", ch );
      return;
    }
    victim->max_mana = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set a PC's practice points 
   */

  if( !str_cmp( arg2, "prac" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 0 || value > 100 )
    {
      send_to_char( "Practice range is 0 to 100 sessions.\n\r", ch );
      return;
    }
    victim->practice = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set a PC's train points 
   */

  if( !str_cmp( arg2, "train" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 0 || value > 100 )
    {
      send_to_char( "Train range is 0 to 100 sessions.\n\r", ch );
      return;
    }
    victim->train = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been reimbursed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set a PC's alignment 
   */

  if( !str_cmp( arg2, "align" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < -1000 || value > 1000 )
    {
      send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
      return;
    }
    victim->alignment = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set a PC's password 
   */

  if( !str_cmp( arg2, "password" ) )
  {
    char *pwdnew;
    char *p;

    if( strlen( arg3 ) < 5 )
    {
      send_to_char( "New password must be at least five characters long.\n\r", ch );
      return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt( arg3, ch->name );
    for( p = pwdnew; *p != '\0'; p++ )
    {
      if( *p == '~' )
      {
        send_to_char( "New password not acceptable, try again.\n\r", ch );
        return;
      }
    }

    DISPOSE( victim->pcdata->pwd );
    victim->pcdata->pwd = str_dup( pwdnew );
    if( IS_SET( sysdata.save_flags, SV_PASSCHG ) )
      save_char_obj( victim );
    send_to_char( "Ok.\n\r", ch );
    ch_printf( victim, "Your password has been changed by %s.\n\r", ch->name );
    return;
  }

  /*
   * Set a PC's mental state 
   */

  if( !str_cmp( arg2, "mental" ) )
  {
    if( value < -100 || value > 100 )
    {
      send_to_char( "Value must be in range -100 to +100.\n\r", ch );
      return;
    }
    victim->mental_state = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set a PC's emotional state 
   */

  if( !str_cmp( arg2, "emotion" ) )
  {
    if( value < -100 || value > 100 )
    {
      send_to_char( "Value must be in range -100 to +100.\n\r", ch );
      return;
    }
    victim->emotional_state = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set a PC's thirst value 
   */

  if( !str_cmp( arg2, "thirst" ) )
  {

    if( value < 0 || value > 100 )
    {
      send_to_char( "Thirst range is 0 to 100.\n\r", ch );
      return;
    }

    victim->pcdata->condition[COND_THIRST] = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set a PC's drunk level 
   */

  if( !str_cmp( arg2, "drunk" ) )
  {

    if( value < 0 || value > 100 )
    {
      send_to_char( "Drunk range is 0 to 100.\n\r", ch );
      return;
    }

    victim->pcdata->condition[COND_DRUNK] = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Set a PC's hunger level 
   */

  if( !str_cmp( arg2, "full" ) )
  {

    if( value < 0 || value > 100 )
    {
      send_to_char( "Full range is 0 to 100.\n\r", ch );
      return;
    }

    victim->pcdata->condition[COND_FULL] = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Change a PC's clan 
   */

  if( !str_cmp( arg2, "clan" ) )
  {
    CLAN_DATA *clan;

    if( !arg3 || arg3[0] == '\0' )
    {
      if( victim->pcdata->clan == NULL )
        return;
      if( !IS_IMMORTAL( victim ) )
      {
        --victim->pcdata->clan->members;
        save_clan( victim->pcdata->clan );
      }
      STRFREE( victim->pcdata->clan_name );
      victim->pcdata->clan_name = STRALLOC( "" );
      victim->pcdata->clan = NULL;
      return;
    }
    clan = get_clan( arg3 );
    if( !clan )
    {
      send_to_char( "No such clan.\n\r", ch );
      return;
    }
    if( victim->pcdata->clan != NULL && !IS_IMMORTAL( victim ) )
    {
      --victim->pcdata->clan->members;
      save_clan( victim->pcdata->clan );
    }
    STRFREE( victim->pcdata->clan_name );
    victim->pcdata->clan_name = QUICKLINK( clan->name );
    victim->pcdata->clan = clan;
    if( !IS_IMMORTAL( victim ) )
    {
      ++victim->pcdata->clan->members;
      save_clan( victim->pcdata->clan );
    }
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Change a PC's deity 
   */

  if( !str_cmp( arg2, "deity" ) )
  {
    DEITY_DATA *deity;

    if( !arg3 || arg3[0] == '\0' )
    {
      STRFREE( victim->pcdata->deity_name );
      victim->pcdata->deity_name = STRALLOC( "" );
      victim->pcdata->deity = NULL;
      send_to_char( "Deity removed.\n\r", ch );
      return;
    }

    deity = get_deity( arg3 );
    if( !deity )
    {
      send_to_char( "No such deity.\n\r", ch );
      return;
    }
    STRFREE( victim->pcdata->deity_name );
    victim->pcdata->deity_name = QUICKLINK( deity->name );
    victim->pcdata->deity = deity;
    send_to_char( "Done.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Change a PC's council 
   */

  if( !str_cmp( arg2, "council" ) )
  {
    COUNCIL_DATA *council;

    if( !arg3 || arg3[0] == '\0' )
    {
      STRFREE( victim->pcdata->council_name );
      victim->pcdata->council_name = STRALLOC( "" );
      victim->pcdata->council = NULL;
      send_to_char( "Removed from council.\n\rPlease make sure you adjust that council's members accordingly.\n\r", ch );
      return;
    }

    council = get_council( arg3 );
    if( !council )
    {
      send_to_char( "No such council.\n\r", ch );
      return;
    }
    STRFREE( victim->pcdata->council_name );
    victim->pcdata->council_name = QUICKLINK( council->name );
    victim->pcdata->council = council;
    send_to_char( "Done.\n\rPlease make sure you adjust that council's members accordingly.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Change a PC's title 
   */

  if( !str_cmp( arg2, "title" ) )
  {
    set_title( victim, arg3 );
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  /*
   * Change a PC's Pre-title 
   */

  if( !str_cmp( arg2, "pretitle" ) )
  {
    STRFREE( victim->pcdata->pretitle );
    victim->pcdata->pretitle = STRALLOC( arg3 );
    send_to_pager_color( "&wOk.\n\r", ch );
    pager_printf_color( victim, "&wYour %s has been changed by %s\n\r", arg2, ch->name );
    return;
  }

  pager_printf_color( ch, "&wError: %s not found.", arg2 );
  return;
}

void do_botwatch( CHAR_DATA * ch, char *argument )
{
  DESCRIPTOR_DATA *d;
  sh_int cswitch = 1;
  CHAR_DATA *vch;
  int lvl = 0;

  lvl = atoi( argument );

  send_to_pager_color( "\n\r&B=----------------------------------------------------------------------------=\n\r", ch );

  for( d = first_descriptor; d; d = d->next )
  {
    vch = d->character;

    if( !vch )
    {
      continue;
    }

    if( vch->pcdata )
    {
      if( get_trust( vch ) > get_trust( ch ) )
        continue;

      if( vch->pcdata->bot_warn[1] >= lvl )
      {
        if( cswitch == 1 )
        {
          pager_printf_color( ch, "&B|&W[%10s][HOURS][%4d] WARNING LEVEL[&C%d/&%c%d]\n\r",
                              vch->name, ( get_age( vch ) - 4 ), vch->pcdata->bot_warn[1],
                              vch->pcdata->bot_warn[2] > 10 ? 'R' :
                              vch->pcdata->bot_warn[2] > 6 ? 'Y' :
                              vch->pcdata->bot_warn[2] > 3 ? 'W' : 'w', vch->pcdata->bot_warn[2] );
          cswitch = 2;
        }
        else
        {
          pager_printf_color( ch, "&B|&Y[%10s][HOURS][%4d] WARNING LEVEL[&C%d/&%c%d]\n\r",
                              vch->name, ( get_age( vch ) - 4 ), vch->pcdata->bot_warn[1],
                              vch->pcdata->bot_warn[2] > 10 ? 'R' :
                              vch->pcdata->bot_warn[2] > 6 ? 'Y' :
                              vch->pcdata->bot_warn[2] > 3 ? 'W' : 'w', vch->pcdata->bot_warn[2] );
          cswitch = 1;
        }
      }
    }

  }
  send_to_pager_color( "&B=----------------------------------------------------------------------------=\n\r", ch );

  return;
}

void do_page( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  argument = one_argument( argument, arg );

  if( arg[0] == '\0' || argument[0] == '\0' )
  {
    send_to_char( "Page who with what?\n\r", ch );
    return;
  }

  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( !can_see( ch, victim ) )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( IS_NPC( victim ) )
  {
    send_to_char( "Mobs don't have pagers.\n\r", ch );
    return;
  }

  pager_printf_color( victim, "&C***&RPAGE&C*** &W%s: %s\n\r\a", ch->name, argument );
  pager_printf_color( ch, "Paging %s.\n\r", victim->name );

  return;
}

void do_pcquesting( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  argument = one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    send_to_char( "Role Play Channel is currently: ", ch );
    if( sysdata.rpChannel )
      send_to_char( "ON\n\r", ch );
    else
      send_to_char( "off\n\r", ch );
    save_sysdata( sysdata );
    return;
  }

  if( !str_cmp( arg, "channel" ) )
  {
    sysdata.rpChannel = !sysdata.rpChannel;

    if( sysdata.rpChannel )
      send_to_char( "RolePlay Channel is now ON.\n\r", ch );
    else
      send_to_char( "RolePlay Channel is now OFF.\n\r", ch );
    save_sysdata( sysdata );
    return;
  }


  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( IS_NPC( victim ) )
  {
    send_to_char( "Mobs are too stupid to role play.\n\r", ch );
    return;
  }

  if( IS_IMMORTAL( victim ) )
  {
    send_to_char( "Not on admins.\n\r", ch );
    return;
  }

  xTOGGLE_BIT( victim->act, PLR_QUESTING );
  if( xIS_SET( victim->act, PLR_QUESTING ) )
    send_to_char( "They are now part of the role playing event.\n\r", ch );
  else
    send_to_char( "They are now NOT part of the role playing event.\n\r", ch );

  return;
}

void do_rppedit( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  int value;
  int i;

  argument = one_argument( argument, arg1 );
  one_argument( argument, arg2 );
  victim = NULL;

  if( arg1[0] == '\0' )
  {
    send_to_char( "'kay, genius. How do you plan to edit RPPs without specifying WHO?\n\r", ch );
    send_to_char( "Syntax: rppedit <name> +/-5\n\r", ch );
    return;
  }

  if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( arg2[0] == '\0' && arg3[0] == '\0' )
  {
    value = victim->pcdata->quest_curr;
    ch_printf( ch, "Target currently has %d RPPs.\n", value );
    value = victim->pcdata->quest_accum;
    ch_printf( ch, "Target has earned %d RPPs total.\n\r", value );
    return;
  }

  if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    send_to_char( "Sorry, boss. Gotta be in the room with 'em.\n\r", ch );
    return;
  }

  if( IS_NPC( victim ) )
  {
    send_to_char( "No.\n\r", ch );
    return;
  }

  if( arg2[0] == '+' || arg2[0] == '-' )
  {
    i = 1;
    while( arg2[i] != '\0' )
    {
      arg3[i - 1] = arg2[i];
      i++;
    }
    value = atoi( arg3 );
//    if ( ( arg2[0] == '+' && ( value == 5 || value == 10 ) )
    if( ( arg2[0] == '+' && ( value == 5 ) ) || ( arg2[0] == '-' && ( value % 5 == 0 ) ) )
    {
      if( IS_NPC( victim ) )
      {
        send_to_char( "No.\n\r", ch );
        return;
      }
      if( get_trust( ch ) < get_trust( victim ) )
      {
        send_to_char( "No.\n\r", ch );
        return;
      }
      if( arg2[0] == '+' )
      {
        victim->pcdata->quest_curr += value;
        victim->pcdata->quest_accum += value;
        ch_printf( ch, "Target has been given %d RPPs.\n\r", value );
        ch_printf( victim, "You have been awarded %d RPPs for your quality roleplaying.\n\r", value );
        return;
      }
      if( arg2[0] == '-' )
      {
        victim->pcdata->quest_curr -= value;
        ch_printf( ch, "Target's RPPs have been reduced by %d.\n\r", value );
        ch_printf( victim, "Your RPPs have been reduced by %d.\n\r", value );
        return;
      }
    }
    else
    {
//      send_to_char("You may only change by increments of 5 or 10.\n\r", ch);
      send_to_char( "You may only change by increments of 5.\n\r", ch );
      return;
    }
  }
  else
  {
    send_to_char( "Must specify whether to add or subtract RPP.\n\r", ch );
    return;
  }
  return;
}

void do_dnsusers( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int count;
  char arg[MAX_INPUT_LENGTH];

  set_pager_color( AT_PLAIN, ch );

  one_argument( argument, arg );
  count = 0;
  buf[0] = '\0';

  sprintf( buf, "\n\rDesc| Con|Idle| Player      @HostDNS          " );
  strcat( buf, "\n\r" );
  strcat( buf, "----+----+----+-------------------------------" );
  strcat( buf, "\n\r" );
  send_to_pager( buf, ch );

  for( d = first_descriptor; d; d = d->next )
  {
    if( arg[0] == '\0' )
    {
      if( get_trust( ch ) >= LEVEL_SUPREME || ( d->character && can_see( ch, d->character ) ) )
      {
        count++;
        sprintf( buf,
                 " %3d| %3d|%4d| %-12s@%-16s ",
                 d->descriptor,
                 d->connected,
                 d->idle / 4, d->original ? d->original->name : d->character ? d->character->name : "(none)", d->host2 );
        strcat( buf, "\n\r" );
        send_to_pager( buf, ch );
      }
    }
    else
    {
      if( ( get_trust( ch ) >= LEVEL_SUPREME
            || ( d->character && can_see( ch, d->character ) ) )
          && ( !str_prefix( arg, d->host ) || ( d->character && !str_prefix( arg, d->character->name ) ) ) )
      {
        count++;

        pager_printf( ch,
                      " %3d| %3d|%4d| %-12s@%-16s ",
                      d->descriptor,
                      d->connected,
                      d->idle / 4,
                      d->original ? d->original->name : d->character ? d->character->name : "(none)", d->host2 );
        buf[0] = '\0';
        if( get_trust( ch ) >= LEVEL_GOD )
          sprintf( buf, "| %s", d->user );
        strcat( buf, "\n\r" );
        send_to_pager( buf, ch );
      }
    }
  }
  return;
}

void do_noscream( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  //char arg[MAX_INPUT_LENGTH];

  set_char_color( AT_IMMORT, ch );

  if( !( victim = get_char_world( ch, argument ) ) || IS_NPC( victim ) )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_IMMORTAL( victim ) )
  {
    send_to_char( "There is no point in noscreaming an immortal.\n\r", ch );
    return;
  }
//  if( !xIS_SET( victim->act, PLR_NOSCREAM ) )
//          ch_printf(victim,"You have been noscreamed and will no longer be able to use most channels.\n\r");
  xTOGGLE_BIT( victim->act, PLR_NOSCREAM );
  save_char_obj( victim );
  ch_printf( ch, "Done.\n\r" );
  return;
}

void do_nognote( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  sh_int time;
  bool h_d = FALSE;
  struct tm *tms;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg );
  if( !*arg )
  {
    send_to_char( "Nognote who, and for how long?\n\r", ch );
    return;
  }
  if( !( victim = get_char_world( ch, arg ) ) || IS_NPC( victim ) )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if( IS_IMMORTAL( victim ) )
  {
    send_to_char( "There is no point in nognoting an immortal.\n\r", ch );
    return;
  }
  if( victim->pcdata->gnote_date != 0 )
  {
    victim->pcdata->gnote_date = 0;
    xREMOVE_BIT( victim->act, PLR_NOGBOARD );
    ch_printf( ch, "%s can use gnotes again.\n\r", victim->name );
    send_to_char( "You can use gnotes again.\n\r", victim );
    return;
  }

  argument = one_argument( argument, arg );
  if( !*arg || !is_number( arg ) )
  {
    if( xIS_SET( victim->act, PLR_NOGBOARD ) )
    {
      ch_printf( ch, "They are nognoted until %24.24s.\n\r", ctime( &victim->pcdata->gnote_date ) );
      return;
    }
    send_to_char( "Nognote them for how long?\n\r", ch );
    return;
  }

  time = atoi( arg );
  if( time <= 0 )
  {
    send_to_char( "You cannot nognote for zero or negative time.\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg );
  if( !*arg || !str_cmp( arg, "hours" ) )
    h_d = TRUE;
  else if( str_cmp( arg, "days" ) )
  {
    send_to_char( "Is that value in hours or days?\n\r", ch );
    return;
  }
  else if( time > 30 )
  {
    send_to_char( "You may not nognote a person for more than 30 days at a time.\n\r", ch );
    return;
  }
  tms = localtime( &current_time );

  if( h_d )
    tms->tm_hour += time;
  else
    tms->tm_mday += time;
  victim->pcdata->gnote_date = mktime( tms );
  xSET_BIT( victim->act, PLR_NOGBOARD );
  ch_printf( ch, "%s will be gnoted at %24.24s.\n\r", victim->name, ctime( &victim->pcdata->gnote_date ) );

  ch_printf( victim, "The immortals are not pleased with your actions.\n\r"
             "You shall not write gnotes for %d %s%s.\n\r", time, ( h_d ? "hour" : "day" ), ( time == 1 ? "" : "s" ) );
  save_char_obj( victim );  /* used to save ch, fixed by Thoric 09/17/96 */
  return;
}

void do_viewbuf( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( ( victim = get_pc_world( ch, argument ) ) == NULL )
  {
    send_to_char( "No such PC.\n\r", ch );
    return;
  }

  if( !victim->desc )
  {
    send_to_char( "No descriptor for target PC.\n\r", ch );
    return;
  }

  send_to_char( "inbuf:\n\r", ch );
  send_to_char( victim->desc->inbuf, ch );
  send_to_char( "\n\r\n\rincomm:\n\r", ch );
  send_to_char( victim->desc->incomm, ch );
  send_to_char( "\n\r\n\rinlast:\n\r", ch );
  send_to_char( victim->desc->inlast, ch );
  send_to_char( "\n\r", ch );
}
void do_chaos( CHAR_DATA * ch, char *argument )
{
/*
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int value = 0;
  //long double valueld = 0;
  int minattr, maxattr;
  bool lockvictim;
  int valuex;
  int sn;

  set_char_color( AT_PLAIN, ch );

  if( IS_NPC( ch ) )
  {
    send_to_char( "Mob's can't godset\n\r", ch );
    return;
  }

  if( !ch->desc )
  {
    send_to_char( "You have no descriptor\n\r", ch );
    return;
  }

  victim = NULL;
  lockvictim = FALSE;
  smash_tilde( argument );

  if( victim )
  {
    lockvictim = TRUE;
    strcpy( arg1, victim->name );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );
  }
  else
  {
    lockvictim = FALSE;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );
  }

  if( arg1[0] == '\0' || !str_cmp( arg1, "?" ) )
  {
    send_to_char( "Syntax: chaos self <field> <value>\n\r", ch );
    send_to_char( "\n\r", ch );
    send_to_char( "Field being one of:\n\r", ch );
    send_to_char( "  pl sdcharge name lname removepk rage\n\r", ch );
    send_to_char( "  stats sex race level zeni energy align\n\r", ch );
    send_to_char( "  pretitle setskill\n\r", ch );
    return;
  }

  if( !victim )
  {
    if( ( victim = get_char_world( ch, arg1 ) ) != ch )
    {
      send_to_char( "Only on yourself, silly.\n\r", ch );
      return;
    }
  }



  minattr = 1;
  maxattr = 100;

  if( !str_cmp( arg2, "pl" ) )
  {
    victim->exp = 100000000000000ULL;
    ch_printf( ch, "Done.\n\r" );
    return;
  }

  if( !str_cmp( arg2, "sdcharge" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    victim->pcdata->sd_charge = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "lname" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    DISPOSE( victim->pcdata->last_name );
    victim->pcdata->last_name = str_dup( "" );
    buf2[0] = ' ';
    strcpy( buf2 + 1, argument );
    victim->pcdata->last_name = strdup( buf2 );
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "removepk" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->pcdata->pk_timer = 0;
    if( xIS_SET( victim->act, PLR_PK2 ) )
      xREMOVE_BIT( victim->act, PLR_PK2 );
    if( xIS_SET( victim->act, PLR_PK1 ) )
      xREMOVE_BIT( victim->act, PLR_PK1 );
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "rage" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    victim->rage = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "stats" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < minattr || value > maxattr )
    {
      ch_printf( ch, "Stat range is %d to %d.\n\r", minattr, maxattr );
      return;
    }
    victim->perm_str = value;
    victim->perm_dex = value;
    victim->perm_int = value;
    victim->perm_con = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "sex" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 0 || value > 2 )
    {
      send_to_char( "Sex range is 0 to 2.\n\r", ch );
      return;
    }
    victim->sex = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "race" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    value = get_pc_race2( arg3 );

    if( value < 0 )
      value = atoi( arg3 );
    if( !IS_NPC( victim ) && ( value < 0 || value >= MAX_RACE ) )
    {
      ch_printf( ch, "Race range is 0 to %d.\n", MAX_RACE - 1 );
      return;
    }

    victim->race = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "level" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    if( value < 0 || value > 13 )
    {
      ch_printf( ch, "Level range is 0 to 13.\n\r" );
      return;
    }
    victim->level = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "zeni" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    victim->gold = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "energy" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < 0 || value > 99999 )
    {
      send_to_char( "Energy range is 0 to 99,999 points.\n\r", ch );
      return;
    }
    victim->mana = value;
    victim->max_mana = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "align" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;
    if( value < -1000 || value > 1000 )
    {
      send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
      return;
    }
    victim->alignment = value;
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "name" ) )
  {
    if( !can_mmodify( ch, victim ) )
      return;

    STRFREE( victim->name );
    victim->name = STRALLOC( arg3 );
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "pretitle" ) )
  {
    STRFREE( victim->pcdata->pretitle );
    victim->pcdata->pretitle = STRALLOC( arg3 );
    send_to_pager_color( "&wOk.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "setskill" ) )
  {
    valuex = 95;
    for( sn = 0; sn < top_sn; sn++ )
    {
      if( skill_table[sn]->min_level[victim->class] == 0 || skill_table[sn]->skill_level[victim->class] == 0 )
        continue;

      if( skill_table[sn]->name && ( victim->exp >= skill_table[sn]->skill_level[victim->class] || valuex == 0 ) )
      {
        if( value > GET_ADEPT( victim, sn ) && !IS_IMMORTAL( victim ) )
          victim->pcdata->learned[sn] = GET_ADEPT( victim, sn );
        else
          victim->pcdata->learned[sn] = valuex;
      }
    }
  }
  pager_printf_color( ch, "&wError: %s not found.", arg2 );
*/
  return;
}

void do_beep( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  argument = one_argument( argument, arg );

  if( !*arg || !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "Beep who?\n\r", ch );
    return;
  }

  /*
   * NPC check added by Samson 2-15-98 
   */
  if( IS_NPC( victim ) )
  {
    send_to_char( "Beep who?\n\r", ch );
    return;
  }

  ch_printf( victim, "%s is beeping you!\a\n\r", PERS( ch, victim ) );
  ch_printf( ch, "You beep %s.\n\r", PERS( victim, ch ) );
  return;
}

void do_dragonballscatter( CHAR_DATA * ch, char *argument )
{

  return;
}
void do_clones(CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *dsrc, *ddst, *dsrc_next, *ddst_next;
	DESCRIPTOR_DATA *dlistf, *dlistl;
	sh_int clone_count;

	set_pager_color( AT_PLAIN, ch );
	pager_printf( ch, " %-12.12s | %-12.12s | %-s\n\r", "characters", "user", "host" );
	pager_printf( ch, "--------------+--------------+---------------------------------------------\n\r" );

	dlistf = dlistl = NULL;

	for( dsrc = first_descriptor ; dsrc ; dsrc = dsrc_next )
	{
		if( (dsrc->character && !can_see(ch, dsrc->character)) 
		|| !dsrc->user || !dsrc->host )
		{
			dsrc_next = dsrc->next;
			continue;
		}

		pager_printf( ch, " %-12.12s |",
			dsrc->original ? dsrc->original->name : 
			(dsrc->character ? dsrc->character->name : "(No name)") );
		clone_count = 1;

		for( ddst = first_descriptor ; ddst ; ddst = ddst_next )
		{
			ddst_next = ddst->next;

			if( dsrc == ddst )
				continue;
			if( (ddst->character && !can_see(ch, dsrc->character)) 
			|| !ddst->user || !ddst->host )
				continue;

			if( !str_cmp( dsrc->user, ddst->user ) 
			&& !str_cmp( dsrc->host, ddst->host ) )
			{
				UNLINK(ddst, first_descriptor, last_descriptor, next, prev );
				LINK( ddst, dlistf, dlistl, next, prev );
				pager_printf( ch, "              |\n\r %-12.12s |",
					ddst->original ? ddst->original->name : 
					(ddst->character ? ddst->character->name : "(No name)") );
				clone_count++;
			}
		}

		pager_printf( ch, " %-12.12s | %s (%d clone%s)\n\r", 
			dsrc->user, dsrc->host,
			clone_count,
			clone_count > 1 ? "s" : "" );

		dsrc_next = dsrc->next;

		UNLINK(dsrc, first_descriptor, last_descriptor, next, prev );
		LINK( dsrc, dlistf, dlistl, next, prev );
	}


	for( dsrc = dlistf ; dsrc ; dsrc = dsrc_next )
	{
		dsrc_next = dsrc->next;
		UNLINK( dsrc, dlistf, dlistl, next, prev );
		LINK(dsrc, first_descriptor, last_descriptor, next, prev );
	}
}

void do_pzap( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_pager_color( AT_RED, ch );

  argument = one_argument( argument, arg );

  if( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_pager( "They aren't here.\n\r", ch );
    return;
  }

  if( IS_NPC( victim ) )
  {
    send_to_char( "You can't zap mobiles! Only online players or immortals!\n\r", ch );
    return;
  }
  else
  {
    send_to_char( "ZAAAAAAAAAAAAAAAAAAAAAAAP!!!!!\n\rOh Shit! You just got zapped, better get in line.\n\r", victim );
    victim->hit = ( victim->max_hit / 3 );
    victim->mana = ( victim->max_mana / 3 );
    return;
  }
  send_to_char( "The target has been zapped successfully!\n\r", ch );
  return;

}


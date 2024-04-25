#!/usr/bin/env python3

#
# This generates enumeration defines used for many purposes,
# as well as tables of strings to store text needed by the core,
# and localized text variations.
#
# Input:
#   TXT files in current folder, starting with settings.txt
# Generates:
#   ../include/nsfplayenums.h
#   ../core/enums_data.h
#

#
# -verify
#   This command line option will not replace the generated files,
#   but instead will parse their contents and compare to the current TXT source,
#   returning 0 if matching, or -1 if not matched.
# -strict
#   Will return -1 at the end of processing if any warnings occured.
#   Can be be used independently of -verify.
# -nowarn
#   Disable warnings.
# -verbose
#   Will print diagnostic information.
#

#
# TXT file format
#   UTF-8
#   each line creates some definition
#   keys are allcaps A-Z/0-9/_ strings with no spaces
#   all text values should be enclosed in quotes, and should never contain quotes
#   # begins a comment, the rest of line will be ignored
#   all integers are signed 32-bit
#
#   A set of options that can be used for an INT setting.
#     LIST list-key key-0 key-1 ...
#
#   A grouping of settings, properties, or song properties
#     GROUP SET group-key
#     GROUP PROP group-key
#     GROUP SONGPROP group-key
#
#   Settings definitions.
#   The SETLIST is like a SETINT but will map its numbers to a LIST.
#   INT settings and properties have a display-type which hints to the implementer
#   how to display that setting.
#     SETINT group-key key default min max hintmin hintmax display-type
#     SETLIST group-key key list-key default-key
#     SETSTR group-key key "default"
#
#   Properties can have an optional comment string in the generated enum.
#   NSF Properties.
#     PROPINT key display-type [comment]
#     PROPLONG key display-type [comment]
#     PROPSTR key [comment]
#     PROPLINES key [comment]
#     PROPBLOB key [comment]
#     PROPLIST key list-key [comment]
#   Song Properties.
#     SONGPROPINT key display-type [comment]
#     SONGPROPLONG key display-type [comment]
#     SONGPROPSTR key [comment]
#     SONGPROPLINES key [comment]
#     SONGPROPBLOB key [comment]
#     SONGPROPLIST key list-key [comment]
#
#   Global channel info.
#   The UNIT will also generate a settings group "UNIT", with a VOL setting.
#   The CHANNEL will also generate ON, VOL, PAN and COL settings. Color should be in hexadecimal form $RRGGBB
#   The CHANNELUNSET can remove an automatically generated setting (CHANNELUNSET SQU1 PAN deletes its PAN setting)
#   The CHANNELONLIST should be used once to specify a LIST to use for the CHANNEL ON settings.
#     UNIT key
#     CHANNEL unit-key key color
#     CHANNELUNSET unit-ley key type
#     CHANNELONLIST list-key
#
#   Locales create a set of localized text for a specific language/locale.
#   Allows nice text to be used instead of the internal keys.
#   The LOCAL command begins a new locale, all LOCAL statements after will apply to it.
#   One locale must be set as the default with LOCALDEFAULT.
#   If a locale is missing strings, it will use the LOCALDEFAULT as a backup.
#   If LOCALDEFAULT is missing strings, it will use the corresponding key if possible.
#   The LOCALPROP applies to both PROP and SONGPROP, as they are really both properties.
#   The LOCALCHANNELSET command should be used once per locale to name the ON/VOL/PAN settings.
#   The LOCALTEXT definition creates an enumerated string, more miscellaneous purposes if needed by the code.
#   LOCALERROR is just LOCALTEXT but the enum will have "ERROR" instead of "TEXT".
#   In place of a text value, * can be used to defer to the LOCALDEFAULT without creating a warning.
#   The LOCALSETLOCALE definition will name/describe a special generated LOCALSET called LOCALE containing all the locales.
#   The generated LOCALSET LOCALE will always be placed in the first GROUP (which must be a GROUP SET).
#     LOCAL key "name"
#     LOCALDEFAULT
#     LOCALLIST list-key key "name"
#     LOCALGROUP group-key "name" "description"
#     LOCALSET key "name" "description"
#     LOCALPROP key "name"
#     LOCALUNIT unit-key "name" "desc"
#     LOCALCHANNEL unit-key key "short-name" "name"
#     LOCALCHANNELSET "enable-name" "volume-name" "pan-name" "col-name" "enable-desc" "volume-desc" "pan-desc" "col-desc"
#     LOCALTEXT "key" "text"
#     LOCALERROR "key" "text"
#     LOCALSETLOCALE "Language" "Display language."
#
#  Other commands. CANCEL skips the rest of a file.
#     CANCEL
#
#  Display types:
#     INT - integer with slider
#     LONG - 64-bit integer (PROPLONG only)
#     BOOL - 0 or 1
#     HEX8/16/32/64 - hexadecimal integer
#     COLOR - RGB value (6 digit hex, or colour picker)
#     MSEC - milliseconds
#     MILL - 1000=100%
#     HZ - audio frequency
#     KEY - keypress code
#     PRECISE - integer with no slider, only manual typing
#

import sys
import os
import shlex
import argparse
import datetime

INPUT_FIRST = "settings.txt"
INPUT_FOLDER = "."
OUTPUT_ENUM = "../include/nsfplayenums.h"
OUTPUT_DATA = "../core/enums_data.h"
TEXT0 = "<MISSING TEXT>"

VERIFY = False
STRICT = False
NOWARN = False
VERBOSE = False

warnings = 0
errors = 0

now_string = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

#
# warnings and errors
#

def print_error(message):
    sys.stderr.write(message + "\n")

def warn(message):
    global warnings
    if not NOWARN:
        print_error("Warning: " + message);
        warnings += 1

def error(message):
    global errors
    print_error("Error: " + message);
    errors += 1

def terminate():
    if STRICT and (warnings > 0): error("Warning treated as error (-strict)")
    if warnings > 0: print_error("%d warnings, check error log" % (warnings))
    if errors   > 0: print_error("%d errors, check error log" % (errors))
    if (errors > 0):
        sys.exit(-1)
    print("Success.")
    sys.exit(0)

def fatal_error(message):
    global errors
    print_error("Fatal error: " + message);
    errors += 1
    #assert(False) # for debugging
    terminate()

def verbose(message):
    if VERBOSE: print(message)

#
# parsing
#

parse_line = 0
parse_path = None

defs_list = []
defs_group = []
defs_set = []
defs_prop = []
defs_unit = []
defs_channel = []
defs_channelunset = []
defs_channelonlist = None
defs_local = []
defs_localdefault = None

PROP_INT    = 1
PROP_LONG   = 2
PROP_STR    = 3
PROP_LINES  = 4
PROP_BLOB   = 5
PROP_LIST   = 6

GT_SET      = 1
GT_PROP     = 2
GT_SONGPROP = 3

DT_INT      = 1
DT_LONG     = 2
DT_STR      = 3
DT_LINES    = 4
DT_BLOB     = 5
DT_LIST     = 6
DT_BOOL     = 7
DT_HEX8     = 8
DT_HEX16    = 9
DT_HEX32    = 10
DT_HEX64    = 11
DT_COLOR    = 12
DT_MSEC     = 13
DT_MILL     = 14
DT_HZ       = 15
DT_KEY      = 16
DT_PRECISE  = 17

GT = { "SET":GT_SET, "PROP":GT_PROP, "SONGPROP":GT_SONGPROP }
GT_REVERSE = {v:k for k,v in GT.items()}

DT = { "INT":DT_INT, "LONG":DT_STR,
       # "LINES":DT_LINES, "BLOB":DT_BLOB, "LIST":DT_LIST,
       "BOOL":DT_BOOL,
       "HEX8":DT_HEX8, "HEX16":DT_HEX16, "HEX32":DT_HEX32, "HEX64":DT_HEX64,
       "COLOR":DT_COLOR, "MSEC":DT_MSEC, "MILL":DT_MILL, "HZ":DT_HZ, "KEY":DT_KEY, "PRECISE":DT_PRECISE }
# the commented values are only set automatically, and aren't valid for SETINT/PROPINT

CHANNEL_ADD = ("ON","VOL","PAN","COL")

# parsing definitions

PARSE_KEY  = 0
PARSE_INT  = 1
PARSE_STR  = 2
PARSE_KEYS = 3
PARSE_DT   = 4
PARSE_GT   = 5

PARSE_DEFS = {
    "LIST":[PARSE_KEY,PARSE_KEYS],
    "GROUP":[PARSE_GT,PARSE_KEY],
    "SETINT":[PARSE_KEY,PARSE_KEY,PARSE_INT,PARSE_INT,PARSE_INT,PARSE_INT,PARSE_INT,PARSE_DT],
    "SETLIST":[PARSE_KEY,PARSE_KEY,PARSE_KEY,PARSE_KEY],
    "SETSTR":[PARSE_KEY,PARSE_KEY,PARSE_STR],
    "PROPINT":[PARSE_KEY,PARSE_KEY,PARSE_DT],
    "PROPLONG":[PARSE_KEY,PARSE_KEY,PARSE_DT],
    "PROPSTR":[PARSE_KEY,PARSE_KEY],
    "PROPLINES":[PARSE_KEY,PARSE_KEY],
    "PROPBLOB":[PARSE_KEY,PARSE_KEY],
    "PROPLIST":[PARSE_KEY,PARSE_KEY,PARSE_KEY],
    "SONGPROPINT":[PARSE_KEY,PARSE_KEY,PARSE_DT],
    "SONGPROPLONG":[PARSE_KEY,PARSE_KEY,PARSE_DT],
    "SONGPROPSTR":[PARSE_KEY,PARSE_KEY],
    "SONGPROPLINES":[PARSE_KEY,PARSE_KEY],
    "SONGPROPBLOB":[PARSE_KEY,PARSE_KEY],
    "SONGPROPLIST":[PARSE_KEY,PARSE_KEY,PARSE_KEY],
    "UNIT":[PARSE_KEY],
    "CHANNEL":[PARSE_KEY,PARSE_KEY,PARSE_INT],
    "CHANNELUNSET":[PARSE_KEY,PARSE_KEY,PARSE_KEY],
    "CHANNELONLIST":[PARSE_KEY],
    "LOCAL":[PARSE_KEY,PARSE_STR],
    "LOCALDEFAULT":[],
    "LOCALLIST":[PARSE_KEY,PARSE_KEY,PARSE_STR],
    "LOCALGROUP":[PARSE_KEY,PARSE_STR,PARSE_STR],
    "LOCALSET":[PARSE_KEY,PARSE_STR,PARSE_STR],
    "LOCALPROP":[PARSE_KEY,PARSE_STR],
    "LOCALUNIT":[PARSE_KEY,PARSE_STR,PARSE_STR],
    "LOCALCHANNEL":[PARSE_KEY,PARSE_KEY,PARSE_STR,PARSE_STR],
    "LOCALCHANNELSET":[PARSE_STR]*len(CHANNEL_ADD)*2,
    "LOCALTEXT":[PARSE_KEY,PARSE_STR],
    "LOCALERROR":[PARSE_KEY,PARSE_STR],
    "LOCALSETLOCALE":[PARSE_STR,PARSE_STR],
    "CANCEL":[],
}
PARSE_DEF_NAME = {
    PARSE_KEY:"KEY",
    PARSE_INT:"INT",
    PARSE_STR:"STRING",
    PARSE_KEYS:"KEY...",
    PARSE_DT:"DISPLAY",
    PARSE_GT:"GROUP-TYPE",
}

# parsing errors

def parse_warn(message):
    if (parse_path != None):
        warn(parse_path + (":%d " % parse_line) + message)
    else:
        warn(message)

def parse_error(message):
    if (parse_path != None):
        error(parse_path + (":%d " % parse_line) + message)
    else:
        error(message)

def is_key(s):
    if len(s) < 1: return False # can't be empty
    if s[0] < 'A' or s[0] > 'Z': # start with letter
        return False
    for c in s: # check allowed characters
        if (c >= 'A' and c <= 'Z'): continue
        if (c >= '0' and c <= '9'): continue
        if (c == '_'): continue
        return False
    return True

# parse defined entry

def parse_entry(ls):
    command = ls[0]
    if command not in PARSE_DEFS:
        error("Unknown command: "+command)
        return (None,None)
    parse_def = PARSE_DEFS[command]
    p = []
    ls = ls[1:]
    for pd in parse_def:
        if len(ls) < 1:
            parse_error(PARSE_DEF_NAME[pd]+" expected: (end of line)")
            return (None,None)
        elif pd == PARSE_KEY:
            if not is_key(ls[0]):
                parse_error(PARSE_DEF_NAME[pd]+" expected: "+ls[0])
                return (None,None)
            p.append(ls[0])
        elif pd == PARSE_INT:
            v = ls[0]
            try:
                radix = 10
                if(v[0] == '$'):
                    radix = 16
                    v = v[1:]
                value = int(v,radix)
            except ValueError:
                parse_error(PARSE_DEF_NAME[pd]+" expected: "+ls[0])
                return (None,None)
            p.append(value)
        elif pd == PARSE_STR:
            if len(ls[0]) < 1:
                parse_error(PARSE_DEF_NAME[pd]+" may not be empty string.")
                return (None,None)
            p.append(ls[0])
        elif pd == PARSE_KEYS: # 0 or more keys
            while len(ls) > 0:
                if not is_key(ls[0]):
                    parse_error(PARSE_DEF_NAME[pd]+" expected: "+ls[0])
                    return (None,None)
                p.append(ls[0])
                ls = ls[1:]
        elif pd == PARSE_GT:
            if ls[0].upper() not in GT:
                parse_error(PARSE_DEF_NAME[pd]+" expected: "+ls[0])
                return (None,None)
            p.append(GT[ls[0].upper()])
        elif pd == PARSE_DT:
            if ls[0].upper() not in DT:
                parse_error(PARSE_DEF_NAME[pd]+" expected: "+ls[0])
                return (None,None)
            p.append(DT[ls[0].upper()])
        ls = ls[1:]
    if len(ls) > 0:
        parse_error(command+" has too many entries, expected: "+str(len(parse_def)))
    return (command,tuple(p))

# validate key references

def check_list(list_key,key=None): # key=None to just check list
    for i in range(len(defs_list)):
        if defs_list[i][0] == list_key:
            if key == None:
                return (i,None,len(defs_list[i])-1)
            for j in range(1,len(defs_list[i])):
                if defs_list[i][j] == key:
                    return (i,j-1,len(defs_list[i])-1)
            parse_error("KEY not found in LIST("+list_key+"): "+key)
            return (None,None,None)
    parse_error("LIST not found: "+list_key)
    return (None,None,None)

def check_group(key,gt):
    for i in range(len(defs_group)):
        if defs_group[i][0] == key:
            if (gt != None) and defs_group[i][1] != gt:
                parse_error("GROUP found but not "+GT_REVERSE[gt]+" type: "+key+" ("+GT_REVERSE[defs_group[i][1]]+")")
                return None
            return i
    parse_error("GROUP not found: "+key)
    return None

def check_set(key):
    for i in range(len(defs_set)):
        if defs_set[i][1] == key:
            return i
    parse_error("SET not found: "+key)
    return None

def check_prop(key):
    for i in range(len(defs_prop)):
        if defs_prop[i][0] == key:
            return i
    parse_error("PROP/SONGPROP not found: "+key)
    return None

def check_unit(key):
    for i in range(len(defs_unit)):
        if defs_unit[i][0] == key: return i
    parse_error("UNIT not found: "+key)
    return None

def check_channel(unit_key,key):
    for i in range(len(defs_unit)):
        if defs_unit[i][0] == unit_key:
            for j in range(len(defs_channel)):
                if defs_channel[j][0] == i and defs_channel[j][1] == key:
                    return (i,j)
            parse_error("CHANNEL not found in UNIT("+unit_key+"): "+key)
            return (None,None)
    parse_error("UNIT not found: "+unit_key)
    return (None,None)   

def add_unique_entry(defs,id_elements,error_name,entry):
    ide = entry[0:id_elements]
    for i in range(len(defs)):
        idi = defs[i][0:id_elements]
        if idi == ide:
            parse_error(error_name + " is not unique")
            return
    defs.append(entry)

def add_prop(p,song=False):
    global defs_prop
    gi = check_group(p[1],GT_SONGPROP if song else GT_PROP)
    if gi != None:
        p = list(p)
        p[1] = gi
        add_unique_entry(defs_prop,1,"PROP/SONGPROP "+p[0],tuple(p))

# parse enums file

def parse_enums(path):
    global defs_list, defs_group, defs_set, defs_prop, defs_unit, defs_channel, defs_channelunset, defs_channelonlist, defs_local, defs_localdefault
    global parse_path, parse_line
    print("Parsing: " + path)
    parse_path = path
    parse_line = 0
    localcurrent = None
    for l in open(path,"rt",encoding="utf-8").readlines():
        parse_line += 1
        try:
            ls = shlex.split(l,comments=True)
        except Exception as e:
            parse_error("Line tokenization error: "+str(e))
            continue
        if len(ls) < 1:
            continue
        (command,p) = parse_entry(ls)
        if command == None: continue
        verbose(command + ": " + str(p))
        if   command == "LIST": # 0-list-key 1-key...
            add_unique_entry(defs_list,1,command+" "+p[0],p)
        elif command == "GROUP": # 0-group-key 1-group-type
            add_unique_entry(defs_group,1,command+" "+p[1],(p[1],p[0]))
        elif command == "SETINT": # 0-group 1-key 2-default 3-min 4-max 5-hint-min 6-hint-max 7-display
            gi = check_group(p[0],GT_SET)
            if gi != None:
                defs_set.append((gi,p[1],p[2],p[3],p[4],p[5],p[6],None,False,p[7])) # set: group, key, default, int min, int max, hint min, hint max, list, is_string, display type
        elif command == "SETLIST": # 0-group 1-key 2-list 3-default
            gi = check_group(p[0],GT_SET)
            if gi != None:
                (li,lk,dcount) = check_list(p[2],p[3])
                if li != None:
                    defs_set.append((gi,p[1],lk,0,dcount-1,0,dcount-1,li,False,DT_LIST))
        elif command == "SETSTR": # 0-group 1-key 2-default
            gi = check_group(p[0],GT_SET)
            if gi != None: defs_set.append((gi,p[1],p[2],0,0,0,0,None,True,DT_STR))
        # PROP
        elif command == "PROPINT":       add_prop((p[1],p[0],PROP_INT,p[2],None)) # 0-key 1-group-key(index) 2-type 3-display 4-list-index
        elif command == "PROPLONG":      add_prop((p[1],p[0],PROP_LONG,p[2],None))
        elif command == "PROPSTR":       add_prop((p[1],p[0],PROP_STR,DT_STR,None))
        elif command == "PROPLINES":     add_prop((p[1],p[0],PROP_LINES,DT_LINES,None))
        elif command == "PROPBLOB":      add_prop((p[1],p[0],PROP_BLOB,DT_BLOB,None))
        elif command == "PROPLIST":
            (li,lk,dcount) = check_list(p[2])
            if (li != None):             add_prop((p[1],p[0],PROP_LIST,DT_LIST,li))
        # SONGPROP
        elif command == "SONGPROPINT":   add_prop((p[1],p[0],PROP_INT,p[2],None),True)
        elif command == "SONGPROPLONG":  add_prop((p[1],p[0],PROP_LONG,p[2],None),True)
        elif command == "SONGPROPSTR":   add_prop((p[1],p[0],PROP_STR,DT_STR,None),True)
        elif command == "SONGPROPLINES": add_prop((p[1],p[0],PROP_LINES,DT_LINES,None),True)
        elif command == "SONGPROPBLOB":  add_prop((p[1],p[0],PROP_BLOB,DT_BLOB,None),True)
        elif command == "SONGPROPLIST":
            (li,lk,dcount) = check_list(p[2])
            if (li != None):             add_prop((p[1],p[0],PROP_LIST,DT_LIST,li),True)
        elif command == "UNIT":
            add_unique_entry(defs_unit,1,"UNIT "+p[0],p)
            add_unique_entry(defs_group,1,"GROUP(UNIT) "+p[0],(p[0],GT_SET))
        elif command == "CHANNEL":
            ui = check_unit(p[0])
            if ui != None:
                defs_channel.append((ui,p[1],p[2]))
        elif command == "CHANNELUNSET":
            (ui,ci) = check_channel(p[0],p[1])
            if ui != None:
                if p[2] not in CHANNEL_ADD:
                    parse_error("CHANNELUNSET must be in %s: %s" % (str(CHANNEL_ADD),p[2]))
                else:
                    defs_channelunset.append((ci,p[2]))
        elif command == "CHANNELONLIST":
            (li,lk,dvcount) = check_list(p[0])
            if li != None:
                if defs_channelonlist != None:
                    parse_error("CHANNELONLIST already used: "+defs_list[defs_channelonlist][0])
                else:
                    defs_channelonlist = li
        elif command == "LOCAL":
            add_unique_entry(defs_local,1,command+" "+p[0],
                [p[0],p[1],[],[],[],[],[],[],None,[],(None,None)]) # local: 0-key, 1-name, 2-list, 3-group, 4-set, 5-prop, 6-unit, 7-channel, 8-channelset, 9-text, 10-locales
            localcurrent = len(defs_local)-1
        elif command == "LOCALDEFAULT":
            if localcurrent == None: parse_error("LOCAL must be used before "+command)
            elif (defs_localdefault != None):
                parse_error("LOCALDEFAULT already used: "+defs_local[defs_localdefault][0])
            else:
                defs_localdefault = localcurrent
        elif command == "LOCALLIST":
            if localcurrent == None: parse_error("LOCAL must be used before "+command)
            else:
                (li,lk,lcount) = check_list(p[0],p[1])
                if li != None:
                    add_unique_entry(defs_local[localcurrent][2],2,command+" "+p[1],(li,lk,p[2])) # list, key, name
        elif command == "LOCALGROUP":
            if localcurrent == None: parse_error("LOCAL must be used before "+command)
            else:
                gi = check_group(p[0],None)
                if gi != None:
                    add_unique_entry(defs_local[localcurrent][3],1,command+" "+p[0],(gi,p[1],p[2])) # group, name, desc
        elif command == "LOCALSET":
            if localcurrent == None: parse_error("LOCAL must be used before "+command)
            else:
                si = check_set(p[0])
                if si != None:
                    add_unique_entry(defs_local[localcurrent][4],1,command+" "+p[0],(si,p[1],p[2])) # set, name, desc
        elif command == "LOCALPROP":
            if localcurrent == None: parse_error("LOCAL must be used before "+command)
            else:
                pi = check_prop(p[0])
                if pi != None:
                    add_unique_entry(defs_local[localcurrent][5],1,command+" "+p[0],(pi,p[1])) # prop, name
        elif command == "LOCALUNIT":
            if localcurrent == None: parse_error("LOCAL must be used before "+command)
            else:
                ui = check_unit(p[0])
                if ui != None:
                    gi = check_group(p[0],GT_SET)
                    if gi == None: fatal_error("UNIT missing automatically generated GROUP: "+p[0])
                    add_unique_entry(defs_local[localcurrent][6],1,command+" "+p[0],(ui,p[1],p[2])) # unit, name, desc
                    add_unique_entry(defs_local[localcurrent][3],1,command+" (GROUP) "+p[0],(gi,p[1],p[2])) # group, name, desc
        elif command == "LOCALCHANNEL":
            if localcurrent == None: parse_error("LOCAL must be used before "+command)
            else:
                (ui,ci) = check_channel(p[0],p[1])
                if ui != None:
                    add_unique_entry(defs_local[localcurrent][7],1,command+" "+p[1],(ci,p[2],p[3])) # channel, shortname, name
        elif command == "LOCALCHANNELSET":
            if localcurrent == None: parse_error("LOCAL must be used before "+command)
            else:
                if defs_local[localcurrent][8] != None:
                    parse_error("LOCALCHANNELSET already used")
                else:
                    defs_local[localcurrent][8] = list(p)
        elif command == "LOCALTEXT":
            if localcurrent == None: parse_error("LOCAL must be used before "+command)
            else:
                add_unique_entry(defs_local[localcurrent][9],1,command+" "+p[0],("TEXT_"+p[0],p[1]))
        elif command == "LOCALERROR":
            if localcurrent == None: parse_error("LOCAL must be used before "+command)
            else:
                add_unique_entry(defs_local[localcurrent][9],1,command+" "+p[0],("ERROR_"+p[0],p[1]))
        elif command == "LOCALSETLOCALE":
            if localcurrent == None: parse_error("LOCAL must be used before "+command)
            else:
                if defs_local[localcurrent][10] != (None,None):
                    parse_error("LOCALSETLOCALE already used")
                else:
                    defs_local[localcurrent][10] = list(p)
        elif command == "CANCEL":
            break
    parse_path = None

def parse_enum_files(files):
    global defs_localdefault, defs_local
    defs_localdefault = None
    for p in files: parse_enums(p)
    # move localdefault to the start of defs_local
    if defs_localdefault == None: fatal_error("No LOCALDEFAULT found")
    defs_local = [defs_local[defs_localdefault]] + defs_local[0:defs_localdefault] + defs_local[defs_localdefault+1:]
    localefault = None
    # ensure other important things are present
    if defs_channelonlist == None: fatal_error("No CHANNELONLIST found")

# generate enums data

gen_enum_lines = None
gen_data_lines = None

gen_text_blob = None
gen_text_map = None

def gen_line(l,target=0):
    global gen_enum_lines, gen_data_lines
    verbose(("%d:" % (target)) + l)
    if   target == 1: gen_data_lines.append(l)
    else:             gen_enum_lines.append(l)

def gen_break(target=0):
    gen_line("",target)

def gen_enum(key,value,target=0,hexadecimal=False):
    if not hexadecimal: gen_line("#define %-40s %12d" % (key,value),target)
    else:               gen_line("#define %-40s %12X" % (key,value),target)

def gen_text(text): # adds utf-8 string to text blob, returns offset to it, duplicates are reused
    global gen_text_blob, gen_text_map
    if text in gen_text_map:
        return gen_text_map[text]
    offset = len(gen_text_blob)
    gen_text_blob += text.encode("utf-8")
    gen_text_blob.append(0)
    gen_text_map[text] = offset
    return offset

def gen_data(data,mode=0,prefix="\t",target=1): # mode: 0=hex bytes (2-digit), 1=int bytes (3-digit), 2=int 4-digit, 3=hex 24-bit (6-digit), 4=int (1-digit)
    i=0
    c=0
    s=""
    FORMS   = ["0x%02X,","%3d,","%4d,","0x%06X,","%1d,"]
    COLUMNS = [ 32,       32,    24,    16,      64   ]
    columns = COLUMNS[mode]
    form    = FORMS[mode]
    for i in range(len(data)):
        s += form % (data[i])
        i += 1
        c += 1
        if (c >= columns):
            gen_line(prefix+s,target)
            s = ""
            c = 0
    if len(s) > 0:
        gen_line(prefix+s,target)       

def generate_enums(file_enum,file_data,do_write):
    global gen_enum_lines, gen_data_lines
    global gen_text_blob, gen_text_map
    global defs_list, defs_group, defs_set, defs_local
    locs = len(defs_local)
    gen_enum_lines = [] # public interface enums
    gen_data_lines = [] # internal data tables
    gen_text_blob = bytearray()
    gen_text_map = {}
    print("Generating data...")
    for i in range(2): gen_line("#pragma once",i)
    for i in range(2): gen_line("// generated by "+os.path.basename(__file__),i)
    for i in range(2): gen_line("// "+now_string,i)
    for i in range(2): gen_break(i)
    gen_line("#include \"" + OUTPUT_ENUM + "\"",1)
    gen_break(1)
    #
    # locale tables contain a byte index to every generated string in gen_data_blob
    # defs_local: key, name, list, group, set, prop, unit, channel, channelset,text
    #
    table_locale = [[] for i in range(locs)]
    for i in range(locs): # add default 0 text to all locales
        table_locale[i].append(gen_text(TEXT0))
    #
    # generate list data
    #
    # generate list for locales and corresponding setting in main group
    LOCALE_KEY = "LOCALE"
    list_locale = [LOCALE_KEY]
    for l in defs_local: list_locale.append(l[0])
    list_locale_index = len(defs_list)
    defs_list.append(list_locale)
    locale_set_index = len(defs_set)
    defs_set.append((0,LOCALE_KEY,0,0,locs-1,0,locs-1,list_locale_index,False,DT_LIST)) # SETLIST: group-0, LOCALE_KEY, default, min, max, hintmin, hintmax, list, not-string, display
    for i in range(len(defs_local)):
        # create the list names in each locale
        for j in range(len(defs_local)):
            defs_local[i][2].append((list_locale_index,j,defs_local[j][1])) # LOCALLIST: LOCALE_KEY, locale-key, locale-name
        # create the set name/desc in each locale
        (name,desc) = defs_local[i][10]
        if name == None or desc == None:
            warn("LOCAL("+defs_local[i][0]+") missing: LOCALSETLOCALE * *")
            (name,desc) = ("*","*")
        defs_local[i][4].append((locale_set_index,name,desc))
    # map each list to a text index, and gather the locale strings to go with it
    gen_enum("NSFP_LIST_COUNT",len(defs_list));
    table_list = []
    for li in range(len(defs_list)):
        list_key = defs_list[li][0]
        keys = list(defs_list[li][1:])
        keys_list = ""
        for i in range(len(keys)): keys_list += keys[i] + "\0"
        keys_list += "\0" # double 0 to end the list
        gen_enum("NSFP_LIST_"+list_key,len(table_list))
        table_list.append(len(table_locale[0])) # locale table index used by table values
        names = [keys[:] for i in range(locs)] # keys used as fallback if no default locale
        for i in range(locs):
            for j in range(len(keys)):
                name = None
                mapped = False
                for (lli,lki,lname) in defs_local[i][2]:
                    if lli == li and lki == j:
                        mapped = True
                        name = lname
                        break
                if not mapped: warn("LOCAL("+defs_local[i][0]+") missing: LOCALLIST "+list_key+" "+keys[j]+" *")
                if name == "*": name = None
                if name != None: names[i][j] = name
                if i == 0: # default locale used as fallback
                    for k in range(1,locs):
                        names[k][j] = names[0][j]
            # the list combines into a multi-string with 0 terminators in between
            local_list = ""
            for j in range(len(keys)): local_list += names[i][j] + "\0"
            local_list += "\0" # double 0 to end the list
            table_locale[i].append(gen_text(keys_list))
            table_locale[i].append(gen_text(local_list))
    gen_break(0)
    gen_line("const int32_t NSFPD_LIST_TEXT[NSFP_LIST_COUNT] = { // text+0 keys, +1 local keys",1)
    gen_data(table_list,mode=2)
    gen_line("};",1)
    gen_break(1)
    # generate list key enums
    for li in range(len(defs_list)):
        list_key = defs_list[li][0]
        keys = list(defs_list[li][1:])
        if list_key == "COUNT": error("LIST name COUNT is reserved")
        gen_enum("NSFP_LK_"+list_key+"_COUNT",len(keys))
        for k in range(len(keys)):
            key = keys[k]
            if key == "COUNT": error("LIST key name COUNT is reserved")
            gen_enum("NSFP_LK_"+list_key+"_"+key,k)
        gen_break(0)
    #
    # generate units
    #
    gen_enum("NSFP_UNIT_COUNT",len(defs_unit))
    table_unit = []
    for ui in range(len(defs_unit)):
        unit_key = defs_unit[ui][0]
        gen_enum("NSFP_UNIT_"+unit_key,ui)
        gi = check_group(unit_key,GT_SET)
        if gi == None: fatal_error("UNIT missing automatically generated GROUP: "+unit_key)
        table_unit.append(gi);
        for i in range(locs):
            mapped = False
            for (lui,name,desc) in defs_local[i][6]:
                if lui == ui:
                    mapped = True
                    break
            if not mapped:
                warn("LOCAL("+defs_local[i][0]+") missing: LOCALUNIT "+unit_key+" * *")
                defs_local[i][3].append((gi,"*","*")) # suppress group warning
    gen_break(0)
    gen_line("const int32_t NSFPD_UNIT_GROUP[NSFP_UNIT_COUNT] = {",1)
    gen_data(table_unit,mode=2)
    gen_line("};",1)
    gen_break(1)
    #
    # generate group data
    #
    # map each group to a text index, gather locale strings
    gen_enum("NSFP_GROUP_COUNT",len(defs_group))
    gen_line("typedef struct {",1)
    gen_line("\tconst char* key;",1)
    gen_line("\tint32_t type, text; // text+0 name, +1 desc",1)
    gen_line("} NSFSetGroupData;",1)
    gen_line("const NSFSetGroupData NSFPD_GROUP[NSFP_GROUP_COUNT] = {",1)
    for gi in range(len(defs_group)):
        group_key = defs_group[gi][0]
        gen_enum("NSFP_GROUP_"+group_key,gi)
        gen_line("\t{ %30s,%4d }," % ('"'+group_key+'"',len(table_locale[0])),1)
        names = [group_key for i in range(locs)]
        descs = [group_key for i in range(locs)]
        for i in range(locs):
            name = None
            desc = None
            mapped = False
            for (lgi,lname,ldesc) in defs_local[i][3]:
                if lgi == gi:
                    mapped = True
                    name = lname
                    desc = ldesc
                    break
            if not mapped: warn("LOCAL("+defs_local[i][0]+") missing: LOCALGROUP "+group_key+" * *")
            if name == "*": name = None
            if desc == "*": desc = None
            if name != None: names[i] = name
            if desc != None: descs[i] = desc
            if i == 0:
                for j in range(1,locs):
                    names[j] = names[0]
                    descs[j] = descs[0]
            table_locale[i].append(gen_text(names[i]))
            table_locale[i].append(gen_text(descs[i]))
    gen_break(0)
    gen_line("};",1)
    gen_break(1)
    #
    # generate setting data
    #
    # generate settings for channels
    gen_enum("NSFP_CHANNEL_COUNT",len(defs_channel))
    gen_line("typedef struct {",1)
    gen_line("\tconst char* key;",1)
    gen_line("\tint32_t unit, text; // text+0 short-name, +1 name",1)
    gen_line("} NSFChannelData;",1)
    gen_line("const NSFChannelData NSFPD_CHANNEL[NSFP_CHANNEL_COUNT] = {",1)
    for i in range(locs):
        CAL = len(CHANNEL_ADD)
        CAL2 = CAL * 2
        if defs_local[i][8] == None:
            warn("LOCAL("+defs_local[i][0]+") missing: LOCALCHANNELSET"+(" *" * CAL2))
            defs_local[i][8] = ["*"] * CAL2
        for j in range(CAL2):
            if defs_local[i][8][j] == "*":
                if i==0: defs_local[i][8][j] = CHANNEL_ADD[i%CAL] # default locale defers to key
                else:    defs_local[i][8][j] = defs_local[0][8][j] # defer to default locale
    for ci in range(len(defs_channel)):
        ui = defs_channel[ci][0]
        unit_key = defs_unit[ui][0]
        gi = check_group(unit_key,GT_SET)
        channel_key = defs_channel[ci][1]
        si = len(defs_set)
        gen_line("\t{ %30s,%2d,%4d }," % ('"'+channel_key+'"',ui,len(table_locale[0])),1)
        gen_enum("NSFP_CHANNEL_"+unit_key+"_"+channel_key,ci)
        CHANNEL_ADD_DEF = [
            (1,0,1,0,1,defs_channelonlist,False,DT_LIST),
            (500,0,8000,0,1000,None,False,DT_MILL),
            (500,0,1000,0,1000,None,False,DT_MILL),
            (defs_channel[ci][2],0,0xFFFFFF,0,0xFFFFFF,None,False,DT_COLOR),
        ]
        assert(CAL==len(CHANNEL_ADD_DEF))
        for i in range(CAL):
            if ((ci,CHANNEL_ADD[i]) not in defs_channelunset):
                defs_set.append(tuple([gi,channel_key+"_"+CHANNEL_ADD[i]]+list(CHANNEL_ADD_DEF[i])))
        shortnames = [channel_key for i in range(locs)]
        names = [channel_key for i in range(locs)]
        for i in range(locs):
            shortname = None
            name = None
            mapped = False
            for j in range(len(defs_local[i][7])):
                if (defs_local[i][7][j][0] == ci):
                    mapped = True
                    shortname = defs_local[i][7][j][1]
                    name = defs_local[i][7][j][2]
            if not mapped: warn("LOCAL("+defs_local[i][0]+") missing: LOCALCHANNEL "+unit_key+" "+channel_key+" *")
            if name == "*": name = None
            if name != None: names[i] = name
            if i == 0:
                for j in range(1,locs):
                    names[j] = names[0]
            if shortname == "*": shortname = None
            if shortname != None: shortnames[i] = shortname
            if i == 0:
                for j in range(1,locs):
                    shortnames[j] = shortnames[0]
            table_locale[i].append(gen_text(shortnames[i]))
            table_locale[i].append(gen_text(names[i]))
            for j in range(CAL):
                defs_local[i][4].append((si+j,names[i]+defs_local[i][8][j+0],names[i]+defs_local[i][8][j+CAL]))
    gen_break(0);
    gen_line("};",1)
    gen_break(1)
    # sort settings by group
    sorted_sets = sorted([(defs_set[i][0],i) for i in range(len(defs_set))])
    # map each setting to a text index, gather locale strings
    gen_enum("NSFP_SET_COUNT",len(defs_set))
    gen_line("typedef struct {",1)
    gen_line("\tconst char* key;",1)
    gen_line("\tint32_t group, text; // text+0 name, +1 desc",1)
    gen_line("\tint32_t default_int, min_int, max_int, min_hint, max_hint;",1)
    gen_line("\tint32_t display, list;",1)
    gen_line("\tconst char* default_str;",1)
    gen_line("} NSFSetData;",1)
    gen_line("const NSFSetData NSFPD_SET[NSFP_SET_COUNT] = {",1)
    setstr_count = 0
    for ssi in range(len(sorted_sets)):
        si = sorted_sets[ssi][1]
        (gi, set_key, default_val, min_int, max_int, min_hint, max_hint, list_index, is_string, display_hint) = defs_set[si]
        group_key = defs_group[gi][0]
        default_int = 0
        default_str = "NULL"
        if is_string: # is_string
            default_str = "\""+default_val+"\""
            default_int = setstr_count # default_int for string settings reused internally as as a lookup index to a string array
            setstr_count += 1
        else:
            default_int = default_val
            if (default_int < min_int) or (default_int > max_int):
                error("SET %s %s default out of range: %d <= %d <= %d" % (group_key,set_key,min_int,default_int,max_int))
            if (min_hint < min_int) or (max_hint > max_int):
                error("SET %s %s hint range outside min/max: %d - %d <> %d - %d" % (group_key,set_key,min_int,max_int,min_hint,max_hint))
        if list_index == None: list_index = -1
        gen_line("\t{ %30s,%3d,%4d,%10d,%11d,%10d,%11d,%10d,%2d,%3d,%s }," % (
            '"'+set_key+'"',
            gi,len(table_locale[0]),
            default_int,min_int,max_int,min_hint,max_hint,
            display_hint,list_index,
            default_str),1)
        gen_enum("NSFP_SET_"+set_key,ssi)
        names = [set_key for i in range(locs)]
        descs = [set_key for i in range(locs)]
        for i in range(locs):
            name = None
            desc = None
            mapped = False
            for (lsi,lname,ldesc) in defs_local[i][4]:
                if lsi == si:
                    mapped = True
                    name = lname
                    desc = ldesc
                    break
            if not mapped: warn("LOCAL("+defs_local[i][0]+") missing: LOCALSET "+set_key+" * *")
            if name == "*": name = None
            if desc == "*": desc = None
            if name != None: names[i] = name
            if desc != None: descs[i] = desc
            if i == 0:
                for j in range(1,locs):
                    names[j] = names[0]
                    descs[j] = descs[0]
            table_locale[i].append(gen_text(names[i]))
            table_locale[i].append(gen_text(descs[i]))
    gen_enum("NSFP_SETSTR_COUNT",setstr_count)
    gen_break(0)
    gen_line("};",1)
    gen_break(1)
    #
    # generate prop data
    #
    # sort props by group
    sorted_props = sorted([(defs_prop[i][1],i) for i in range(len(defs_prop))])
    gen_enum("NSFP_PROP_COUNT",len(defs_prop))
    gen_line("typedef struct {",1)
    gen_line("\tconst char* key;",1)
    gen_line("\tint32_t group, text, type, display; // text+0 name, +1 desc",1)
    gen_line("\tint32_t max_list, list;",1)
    gen_line("} NSFPropData;",1)
    gen_line("const NSFPropData NSFPD_PROP[NSFP_PROP_COUNT] = {",1)
    for spi in range(len(sorted_props)):
        pi = sorted_props[spi][1]
        (prop_key,gi,prop_type,prop_display,prop_list) = defs_prop[pi]
        prop_group_type = defs_group[gi][1]
        list_index = -1
        list_max = 0
        if prop_list != None:
            list_index = prop_list
            list_max = len(defs_list[list_index])-2
        gen_line("\t{ %30s,%2d,%4d,%1d,%2d,%3d,%3d }," % (
            '"'+prop_key+'"',
            gi,len(table_locale[0]),prop_type,prop_display,
            list_max,list_index),1)
        gen_enum("NSFP_PROP_"+prop_key,spi);
        names = [prop_key for i in range(locs)]
        for i in range(locs):
            name = None
            mapped = False
            for (lpi,lname) in defs_local[i][5]:
                if lpi == pi:
                    mapped = True
                    name = lname
                    break
            if not mapped: warn("LOCAL("+defs_local[i][0]+") missing: LOCALPROP "+prop_key+" *")
            if name == "*": name = None
            if name != None: names[i] = name
            if i == 0:
                for j in range(1,locs):
                    names[j] = names[0]
            table_locale[i].append(gen_text(names[i]))
    gen_break(0)
    gen_line("};",1)
    gen_break(1)
    #
    # generate extra text
    #
    for ti in range(len(defs_local[0][9])):
        text_key = defs_local[0][9][ti][0]
        gen_enum("NSFP_"+text_key,len(table_locale[0]))
        names = [text_key for i in range(locs)]
        for i in range(locs):
            name = None
            mapped = False
            for (lkey,lname) in defs_local[i][9]:
                if lkey == text_key:
                    mapped = True
                    name = lname
                    break
            if not mapped: warn("LOCAL("+defs_local[i][0]+") missing: LOCALTEXT "+text_key+" *")
            if name == "*": name = None
            if name != None: names[i] = name
            table_locale[i].append(gen_text(names[i]))
    # verify there aren't stray TEXT definitions outside the default locale
    for i in range(1,locs):
        for (text_key,text_name) in defs_local[i][9]:
            mapped = False
            for j in range(len(defs_local[0][9])):
                if text_key == defs_local[0][9][j][0]:
                    mapped = True
                    break
            if not mapped: error("LOCAL("+defs_local[i][0]+") LOCALTEXT not in default LOCAL: "+text_key)
    #
    # generate text data tables
    #
    gen_enum("NSFP_TEXT_COUNT",len(table_locale[0]))
    gen_break(0)
    gen_enum("NSFP_LOCALE_COUNT",len(defs_local))
    gen_line("#if !(NSFP_NOTEXT)",1)
    gen_break(1)
    gen_line("const int32_t NSFPD_LOCAL_TEXT[NSFP_LOCALE_COUNT][%d] = {" % (len(table_locale[0])),1)
    for i in range(0,locs):
        gen_enum("NSFP_LOCALE_"+defs_local[i][0],i)
        gen_line("{",1)
        gen_data(table_locale[i],mode=3)
        gen_line("},",1)
    gen_line("};",1)
    gen_break(1)
    gen_line("const uint8_t NSFPD_LOCAL_TEXT_DATA[0x%06X] = {" % (len(gen_text_blob)),1)
    gen_data(gen_text_blob,mode=0)
    gen_line("};",1)
    gen_break(1)
    gen_line("#else // (NSFP_NOTEXT)",1)
    gen_break(1)
    max_list_len = 2;
    for dl in defs_list:
        if len(dl) > max_list_len: max_list_len = len(dl) # this is actually +1 because dl contains the list name as well, but we need the extra 0 for the double terminal
    gen_line("const uint8_t NSFPD_NOTEXT_LIST_KEY[%d] = {" % (max_list_len),1)
    gen_data([0]*max_list_len,mode=4);
    gen_line("};",1)
    gen_break(1)
    gen_line("#endif",1)
    gen_break(1);
    for i in range(1,locs):
        if (len(table_locale[i]) != len(table_locale[0])):
            error("Internal error? LOCAL(%s) has mismatched count: %d != %d" % (defs_local[i][0],len(table_locale[i]),len(table_locale[0])))
    #
    # save the data
    #
    for i in range(2): gen_line("// end of file",i)
    if (do_write):
        print("Writing: "+file_enum)
        with open(file_enum,"wt") as f:
            for l in gen_enum_lines: f.write(l+"\n")
        print("Writing: "+file_data)
        with open(file_data,"wt") as f:
            for l in gen_data_lines: f.write(l+"\n")
    print("Generate finished.")

# verify generated enums data

def ignore_line(l):
    ls = l.strip()
    if len(ls) < 1: return True
    if ls.startswith("//"): return True
    if ls.startswith("#pragma"): return True
    if ls.startswith("#include"): return True
    return False

def verify_lines(lines0,file):
    global parse_path, parse_line
    print("Verify: "+file)
    parse_path = file
    lines1 = open(file,"rt").readlines()
    i0 = 0
    i1 = 0
    while i0 < len(lines0) and i1 < len(lines1):
        if ignore_line(lines0[i0]):
            i0 += 1
            continue
        if ignore_line(lines1[i1]):
            i1 += 1
            continue
        if lines0[i0].strip() != lines1[i1].strip():
            parse_line = i1 + 1
            parse_error("Verify failed (generated:%d)" % (i0+1))
            parse_path = None
            return False
        i0 += 1
        i1 += 1
    while i1 < len(lines1):
        if not ignore_line(lines1[i1]):
            parse_line = i1 + 1
            parse_error("Verify failed, extra line")
            parse_path = None
            return False
        i1 += 1
    parse_path = None
    while i0 < len(lines0):
        if not ignore_line(lines0[i0]):
            error("Verify failed, extra line at generated:%d" % (i0+1))
            return False
        i0 += 1
    return True

def verify_enums(file_enum,file_data):
    result = True
    result &= verify_lines(gen_enum_lines,file_enum)
    result &= verify_lines(gen_data_lines,file_data)
    if not result:
        print_error("Verification failed, try running "+os.path.basename(__file__)+" without parameters (make enums) to rebuild first?")
    return result

#
# main
#

if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("-verify",action="store_true",help="Suppress output, but verify match with existing generated files.")
    ap.add_argument("-strict",action="store_true",help="Treat warnings as errors.")
    ap.add_argument("-nowarn",action="store_true",help="Disable all warnings.")
    ap.add_argument("-verbose",action="store_true",help="Verbose diagnostic output.")
    args = ap.parse_args()
    VERIFY = args.verify
    STRICT = args.strict
    NOWARN = args.nowarn
    VERBOSE = args.verbose

if VERIFY: print("Option: -verify")
if STRICT: print("Option: -strict")
if NOWARN: print("Option: -nowarn")
if VERBOSE: print("Option: -verbose")

input_files = [os.path.join(INPUT_FOLDER,INPUT_FIRST)]
for p in sorted(os.listdir(INPUT_FOLDER)): # sort alphabetically for stable locale order
    if (p.lower().endswith(".txt") and p != INPUT_FIRST):
        input_files.append(os.path.join(INPUT_FOLDER,p))

print("Parsing %d files..." % (len(input_files)))
parse_enum_files(input_files)

generate_enums(OUTPUT_ENUM,OUTPUT_DATA,not VERIFY)
verify_enums(OUTPUT_ENUM,OUTPUT_DATA)

terminate()

# This script scans the polcodesnippet liquid filters embedded in the markdown files 
# and makes a dictionary of all tags that should appear in any source code file.
# It also scans source code files and reports any tags that are not being used as well
# as tags that are used in markdown files but do not appear in the source code file.
#
# Here are some examples of polcodesnippet filters:
#
# {{ "steps/step-02/src/node.hpp" | polcodesnippet:"start-end","" }}
# {{ "steps/step-03/src/node.hpp" | polcodesnippet:"start-end","a,b"}}
#
# Key:
# {{ <--- source code file -----> | polcodesnippet:"lines shown","lines highlighted" }}
# 

import re,sys,glob,os

def separateTags(tagstr, highlight):
    # Examples of tagstr:
    #   start-end
    #   a,b-c,e-f
    #   !a,b        
    tagvect = []
    parts = tagstr.split(',')
    for p in parts:
        if '-' in p:
            subparts = p.split('-')
            assert len(subparts) == 2
            s0 = subparts[0].strip()
            s1 = subparts[1].strip()
            if highlight:
                tagvect.append('!'+s0)
                tagvect.append('!'+s1)
            else:
                tagvect.append(s0)
                tagvect.append(s1)
        else:
            p = p.strip()
            if highlight:
                tagvect.append('!'+p)
            else:
                tagvect.append(p)
    return tagvect

tags_in_markdown = {}
tags_in_sources = {}
for step_number in ['01','02','03','04','05','06','07','08','09','10','11','12','13','14','15','16','17','18','19']:
    step_dir = '/Users/plewis/Documents/websites/stromtutorial.github.io/steps/step-%2s' % step_number

    # First, scan markdown files and create a dictionary tags_in_markdown with 
    #      key =  <step number>-<source file name>
    #      value = {'tags':[<tags referenced>], 'md':[<md file(s)>]}
    files = glob.glob(os.path.join(step_dir, '*.md'))
    files.sort()

    for pathname in files:
        mddir,mdfn = os.path.split(pathname)
        lines = open(pathname, 'r').readlines()
        for line in lines:
            m = re.match('{{ "steps/step-(\d\d)/src/(.+?)" [|] polcodesnippet:"(.*?)"\s*,\s*"(.*)"', line)
            if m is not None:
                source_step = m.group(1)
                source_file = m.group(2)
                shown       = m.group(3)
                highlighted = m.group(4)

                key = source_step + '-' + source_file
                    
                if key in tags_in_markdown.keys():
                    if mdfn not in tags_in_markdown[key]['md']:
                        tags_in_markdown[key]['md'].append(mdfn)
                else:
                    tags_in_markdown[key] = {'tags':set(), 'md':[mdfn]}

                if len(shown) > 0:
                    shown_vect = separateTags(shown, False)
                    for s in shown_vect:
                        tags_in_markdown[key]['tags'].add(s)
                        
                if len(highlighted) > 0:
                    highlighted_vect = separateTags(highlighted, True)
                    for h in highlighted_vect:
                        tags_in_markdown[key]['tags'].add(h)
                        
    # Second, scan source code files to catalogue all tags used
    hppfiles  = glob.glob(os.path.join(step_dir, 'src/*.hpp'))
    cppfiles  = glob.glob(os.path.join(step_dir, 'src/*.cpp'))
    source_code_files = hppfiles + cppfiles
    source_code_files.sort()
        
    # Handle the hpp and cpp files first
    for pathname in source_code_files:
        mddir,fn = os.path.split(pathname)
        stuff = open(pathname, 'r').read()
        tags_found = re.findall('///(\S+)', stuff, re.S | re.M)
        if len(tags_found) > 0:
            key = step_number + '-' + fn
            if key not in tags_in_sources.keys():
                tags_in_sources[key] = {'tags':set(), 'dups':set()}
            for t in tags_found:
                if t in tags_in_sources[key]['tags']:
                    tags_in_sources[key]['dups'].add(t)
                else:
                    tags_in_sources[key]['tags'].add(t)

    # Now do the meson.build file                    
    mesonfile = os.path.join(step_dir, 'src/meson.build')
    stuff = open(mesonfile, 'r').read()
    
    tags_found = re.findall('###(\S+)', stuff, re.S | re.M)
    if len(tags_found) > 0:
        mddir,fn = os.path.split(mesonfile)
        key = step_number + '-' + fn
        if key not in tags_in_sources.keys():
            tags_in_sources[key] = {'tags':set(), 'dups':set()}
        for t in tags_found:
            if t in tags_in_sources[key]['tags']:
                tags_in_sources[key]['dups'].add(t)
            else:
                tags_in_sources[key]['tags'].add(t)

mdkeys = tags_in_markdown.keys()[:]
mdkeys.sort()

cppkeys = tags_in_sources.keys()[:]
cppkeys.sort()

mdkeyset = set(mdkeys)        
cppkeyset = set(cppkeys)

if True:
    diff = mdkeyset.symmetric_difference(cppkeyset)
    difflist = list(diff)
    difflist.sort()
    print
    print '***** Files with tags in markdown filters but not in source'
    for k in difflist:
        if k in mdkeyset:
            assert not k in cppkeyset
            print k
    print
    print '****** Files with tags in source filters but not in markdown'
    for k in difflist:
        if k in cppkeyset:
            assert not k in mdkeyset
            print k
    print
    
if True:
    colw   = 35
    plusfmt = '  +%%%ds+%%%ds+%%%ds+' % (colw, colw, colw)
    sidefmt = '  |%%%ds|%%%ds|%%%ds|' % (colw, colw, colw)

    wleft  = colw//2 - 2
    wright = colw - wleft - 4
    mdheader  = '-'*wleft + ' md '  + '-'*wright
    mdbottom  = '-'*colw
    
    wleft  = colw//2 - 2
    wright = colw - wleft - 5
    cppheader = '-'*wleft + ' cpp ' + '-'*wright
    cppbottom  = '-'*colw

    wleft  = colw//2 - 2
    wright = colw - wleft - 6
    dupheader = '-'*wleft + ' dups ' + '-'*wright
    dupbottom  = '-'*colw

    mdxcpp = mdkeyset.intersection(cppkeyset)
    mdxcpplist = list(mdxcpp)
    mdxcpplist.sort()
    print '****** Files with tags in both markdown filters and in source'
    for f in mdxcpplist:
        x = tags_in_sources[f]['tags']
        duplicates = tags_in_sources[f]['dups']
        y = tags_in_markdown[f]['tags']
        z = tags_in_markdown[f]['md']
        diff = x.symmetric_difference(y)
        if len(diff) == 0 and len(duplicates) == 0:
            print ' ',f
        else:
            zz = ','.join(['%s' % zz for zz in z])
            print '* %s (%s)' % (f,zz)
            print plusfmt % (mdheader, cppheader, dupheader)
            for d in diff:
                dup = d in duplicates
                print sidefmt % (d in y and d or '-', d in x and d or '-', dup and d or '')
            for d in duplicates:
                if not d in diff:
                    print sidefmt % (d in y and d or '-', d in x and d or '-', d)
            print plusfmt % (mdbottom, cppbottom, dupbottom)

if True:
    print '****** Tags found in markdown files:'
    for k in mdkeys:
        print '  %s: ' % k
        for t in tags_in_markdown[k]['tags']:
            print '   %s' % t
        for f in tags_in_markdown[k]['md']:
            print '   --> %s' % f
        print
    print
    print '****** Tags found in source code files:'
    for k in cppkeys:
        print '  %s: ' % k
        for t in tags_in_sources[k]['tags']:
            print '   %s' % t
        print


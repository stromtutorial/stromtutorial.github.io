# Copies steps/step-09/src/node.hpp (for example) to <target-dir>/node09.hpp, stripping all
# markdown special comments (e.g. "///!b") so that file comparisons between steps are 
# not cluttered by these tags. Having a flattend file hierarchy makes it easy to open, say,
# node02.hpp, node03.hpp, ..., node19.hpp in BBEdit to easily compare the same file across
# different steps.

import re,sys,glob,os

assert len(sys.argv) == 2, 'usage: python stripmarkdowntags.py <target-dir>'
targetdir = sys.argv[1]

# Abort if target directory already exists
ok = os.path.exists(targetdir)
if not ok:
    sys.exit('Specified target directory does not exist')
    
# Abort if target directory contains files
targetdir_contents = glob.glob(os.path.join(targetdir,'*'))
if len(targetdir_contents) > 0:
    print 'Specified target directory ("%s") is not empty:' % targetdir
    for t in targetdir_contents:
        print '  ',t
    sys.exit()

# Create list of src directories to process
steppaths = glob.glob('../steps/*/src')
steppaths.sort()

for sp in steppaths:
    # Extract step number from src directory path name
    m = re.search('step-(\d\d)', sp)
    assert m is not None
    step = m.group(1)

    # extract source file names within src directory
    filepaths = glob.glob(os.path.join(sp,'*'))
    for fp in filepaths:
        # Extract file name from file path
        fn = os.path.basename(fp) 
        
        # Create new file name combining original name and step number
        mm = re.match('(\S+)[.](\S+)', fn)
        assert mm is not None
        beforedot = mm.group(1)
        afterdot  = mm.group(2)
        newfn = '%s%s.%s' % (beforedot, step, afterdot)
        
        # Create file path to new file name
        newfp = os.path.join(targetdir,newfn)
        
        # strip the original file and save at new file path
        lines = open(fp, 'r').readlines()
        contents = ''
        ntagsreplaced = 0
        for line in lines:
            stripped,n = re.subn('\s*///\S+[ \t]*', '', line)
            ntagsreplaced += n
            contents += stripped
        print '%d <-- %s' % (ntagsreplaced, newfn)
        open(newfp, 'w').write(contents)

__license__   = 'GPL v3'
__copyright__ = '2008, Kovid Goyal <kovid at kovidgoyal.net>'

import os, sys, shutil, glob, logging
from tempfile import mkdtemp
from subprocess import Popen, PIPE
from libprs500.ebooks.lrf import option_parser as lrf_option_parser
from libprs500.ebooks import ConversionError
from libprs500.ebooks.lrf.html.convert_from import process_file as html_process_file
from libprs500.ebooks.metadata.opf import OPFReader
from libprs500 import isosx, __appname__, setup_cli_handlers, iswindows

CLIT = 'clit'
if isosx and hasattr(sys, 'frameworks_dir'):
    CLIT = os.path.join(getattr(sys, 'frameworks_dir'), CLIT)

def option_parser():
    return lrf_option_parser(
_('''Usage: %prog [options] mybook.lit


%prog converts mybook.lit to mybook.lrf''')
        )

def generate_html(pathtolit, logger):
    if not os.access(pathtolit, os.R_OK):
        raise ConversionError, 'Cannot read from ' + pathtolit
    tdir = mkdtemp(prefix=__appname__+'_')
    os.rmdir(tdir)
    sep = r'\\' if iswindows else os.path.sep
    cmd = ' '.join([CLIT, '"'+pathtolit+'"', '"%s"'%(tdir+sep,)])
    logger.debug(cmd)
    p = Popen(cmd, shell=True, stderr=PIPE, stdout=PIPE)
    stdout = p.stdout.read()
    err = p.stderr.read()     
    logger.info(p.stdout.read())
    ret = p.wait()
    if ret != 0:
        if os.path.exists(tdir) and os.path.isdir(tdir):
            shutil.rmtree(tdir)        
        if 'keys.txt' in unicode(err)+unicode(stdout):
            raise ConversionError('This lit file is protected by DRM. You must first use the ConvertLIT program to remove the DRM. Doing so may be illegal, and so %s does not do this, nor does it provide instructions on how to do it.'%(__appname__,))
        raise ConversionError, err
    return tdir

def process_file(path, options, logger=None):
    if logger is None:
        level = logging.DEBUG if options.verbose else logging.INFO
        logger = logging.getLogger('lit2lrf')
        setup_cli_handlers(logger, level)
    lit = os.path.abspath(os.path.expanduser(path))
    tdir = generate_html(lit, logger)
    try:
        opf = glob.glob(os.path.join(tdir, '*.opf'))
        if opf:
            path = opf[0]
            opf = OPFReader(path)
            htmlfile = opf.spine.items().next().href.replace('&', '%26') #convertlit replaces & with %26
            options.opf = path
        else:    
            l = glob.glob(os.path.join(tdir, '*toc*.htm*'))
            if not l:
                l = glob.glob(os.path.join(tdir, '*top*.htm*'))
            if not l:
                l = glob.glob(os.path.join(tdir, '*contents*.htm*'))
            if not l:
                l = glob.glob(os.path.join(tdir, '*.htm*'))
                if not l:
                    l = glob.glob(os.path.join(tdir, '*.txt*')) # Some lit file apparently have .txt files in them
                    if not l:
                        raise ConversionError('Conversion of lit to html failed. Cannot find html file.')
                maxsize, htmlfile = 0, None
                for c in l:
                    sz = os.path.getsize(c)
                    if sz > maxsize:
                        maxsize, htmlfile = sz, c
            else:
                htmlfile = l[0]
        if not options.output:
            ext = '.lrs' if options.lrs else '.lrf'
            options.output = os.path.abspath(os.path.basename(os.path.splitext(path)[0]) + ext)
        options.output = os.path.abspath(os.path.expanduser(options.output))
        options.use_spine = True
        
        html_process_file(htmlfile, options, logger=logger)
    finally:
        try:
            shutil.rmtree(tdir)
        except:
            logger.warning('Failed to delete temporary directory '+tdir)


def main(args=sys.argv, logger=None):
    parser = option_parser()
    options, args = parser.parse_args(args)
    if len(args) != 2:            
        parser.print_help()
        print
        print 'No lit file specified'
        return 1
    process_file(args[1], options, logger)
    return 0        
        
            
if __name__ == '__main__':
    sys.exit(main())

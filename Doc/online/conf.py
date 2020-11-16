# -*- coding: utf-8 -*-
#
# Ecflow Tutorial documentation build configuration file, created by
# sphinx-quickstart on Mon Feb 28 11:23:56 2011.
#
# This file is execfile()d with the current directory set to its containing dir.
#
# Note that not all possible configuration values are present in this
# autogenerated file.
#
# All configuration values have a default; values that are commented out
# serve to show the default.

import sys, os

def get_installed_ecflow_version():
    "This will extract ecFlow version from /usr/local/apps/ecflow/current."
    "Will return a list of form `[2,0,24]`"
    ecflow_dir_ext = os.readlink("/usr/local/apps/ecflow/current")
    # print(ecflow_dir_ext)
    version_list = ecflow_dir_ext.split(".")
    assert len(version_list) == 3 ,"Expected 3 items,release,major,minor but found " + ecflow_dir_ext
    print("Extracted ecflow version from /usr/local/apps/ecflow/current: " + ecflow_dir_ext)
    return version_list;
    
def get_ecflow_version( work_space ):
    "This will extract ecFlow version *list* from the source code."
    "The version is defined in the file CMakeList.txt"
    "expecting string of form:"
    "project( ecflow LANGUAGES CXX VERSION 5.6.0 )"
    file = work_space + "/CMakeLists.txt"
    ecflow_version = []
    if os.path.exists(file):
        cmake_file = open(file,'r')
        try :
            for line in cmake_file :
                project = line.find('project(')
                ecflow = line.find('ecflow')
                cxx = line.find('CXX')
                version_t = line.find('VERSION')
                if project != -1 and ecflow != -1 and cxx != -1 and version_t != -1 :
                    tokens = line.split()
                    version_index = tokens.index("VERSION")
                    version = tokens[version_index+1]
                    if version[-1] == ')':
                        version = version[:-1]
                    ecflow_version = version.split(".")
        finally:
            cmake_file.close();
        
        print("Extracted ecflow version '" + str(ecflow_version) + "' from " + file)
        return ecflow_version
    else:
        return get_installed_ecflow_version()
    
WK="../.."

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#sys.path.insert(0, os.path.abspath('.'))

# -- General configuration -----------------------------------------------------

# If your documentation needs a minimal Sphinx version, state it here.
#needs_sphinx = '1.0'

# Add any Sphinx extension module names here, as strings. They can be extensions
# coming with Sphinx (named 'sphinx.ext.*') or your custom ones.
# avi>> append with path to ecflow extension. relies on Pyext installing ecflow in ecflow/ directory
#
# ISSUES 0:
#   Why can't I pick up changes in doc strings for ecflow extension, until it is installed
#   1/ sphinx will use python path, hence pick up from PYTHONPATH/LD_LIBRARY_PATH ( needed for boost python)
#   2/ This can be verified by removing ecflow from LD_LIBRARY_PATH, and running make html again
#      By inserting Pyext/ecflow at position 0, we will look here first:
#   FIXED: By inserting rather than appending to sys.path
#
# ISSUES 1:
#   It appears the sphinx expects the  shared lib to be named ecflow, and *NOT* _ecflow
#   this shows that sphinx is *NOT* loading the the module as a package, i.e it does not load __init__.py
#   Hence can not document class/functions __init__.py in this file
#
#   ***NOTE***: if we use sys.path.insert(0,os.getcwd() + "/../../Pyext")
#               Then we can document functions in __init__.py but *NOT* the extension ??
#    

# If ecflow.so does not exist, then the ecflow python reference will NOT resolve. No warnings are given
# Hence added check
if not os.path.exists("../../../bdir/release/ecflow/Pyext/python3/ecflow.so"):
    print("\necflow extension not built: ecflow.so missing from directory ../../bdir/release/ecflow/Pyext/ecflow.so")
    sys.exit(1)
    
sys.path.insert(0,os.getcwd() + "../../../bdir/release/ecflow/Pyext/python3/ecflow.so")   
#sys.path.insert(0,os.getcwd() + "/../../Pyext")   
#print "sys.path: " + str(sys.path)
extensions = ['sphinx.ext.viewcode' , 'sphinx.ext.autodoc' ]


# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# The suffix of source filenames.
source_suffix = '.rst'

# The encoding of source files.
#source_encoding = 'utf-8-sig'

# The master toctree document.
master_doc = 'index'

# General information about the project.
project = u'ecFlow'
copyright = u'2017, E.C.M.W.F'

# The version info for the project you're documenting, acts as replacement for
# |version| and |release|, also used in various other places throughout the
# built documents.
#
# The short X.Y version.
version = '1.0'

# The full version, including alpha/beta/rc tags.
ecflow_version_list = get_ecflow_version( WK )
assert len(ecflow_version_list) == 3, "Expected version to have release, major,minor"
release = ecflow_version_list[0] + "." + ecflow_version_list[1] + "." + ecflow_version_list[2]

# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#language = None

# There are two options for replacing |today|: either, you set today to some
# non-false value, then it is used:
#today = ''
# Else, today_fmt is used as the format for a strftime call.
#today_fmt = '%B %d, %Y'

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
exclude_patterns = ['_build']

# The reST default role (used for this markup: `text`) to use for all documents.
#default_role = None

# If true, '()' will be appended to :func: etc. cross-reference text.
#add_function_parentheses = True

# If true, the current module name will be prepended to all description
# unit titles (such as .. function::).
#add_module_names = True

# If true, sectionauthor and moduleauthor directives will be shown in the
# output. They are ignored by default.
#show_authors = False

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'

# A list of ignored prefixes for module index sorting.
#modindex_common_prefix = []


# -- Options for HTML output ---------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes. { default | sphinxdoc | scrolls | agogo, etc }
html_theme = 'default'

# Theme options are theme-specific and customize the look and feel of a theme
# further.  For a list of options available for each theme, see the
# documentation.
#html_theme_options = {}

# Add any paths that contain custom themes here, relative to this directory.
#html_theme_path = []

# The name for this set of Sphinx documents.  If None, it defaults to
# "<project> v<release> documentation".
#html_title = None

# A shorter title for the navigation bar.  Default is the same as html_title.
#html_short_title = None

# The name of an image file (relative to this directory) to place at the top
# of the sidebar.
#html_logo = None

# The name of an image file (within the static path) to use as favicon of the
# docs.  This file should be a Windows icon file (.ico) being 16x16 or 32x32
# pixels large.
#html_favicon = None

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# If not '', a 'Last updated on:' timestamp is inserted at every page bottom,
# using the given strftime format.
#html_last_updated_fmt = '%b %d, %Y'

# If true, SmartyPants will be used to convert quotes and dashes to
# typographically correct entities.
#html_use_smartypants = True

# Custom sidebar templates, maps document names to template names.
#html_sidebars = {}
html_sidebars = {
   'index'                : ['localtoc.html', 'relations.html', 'sourcelink.html', 'searchbox.html' ],
   'tutorial/tutorial'    : ['globaltoc.html','relations.html', 'sourcelink.html', 'searchbox.html' ],
   'install/install'      : ['localtoc.html', 'relations.html', 'sourcelink.html', 'searchbox.html' ],
   'faq/faq'              : ['localtoc.html', 'relations.html', 'sourcelink.html', 'searchbox.html' ],
   'grammar/grammar'      : ['localtoc.html', 'relations.html', 'sourcelink.html', 'searchbox.html' ],
   'python_api/python_api': ['localtoc.html', 'relations.html', 'sourcelink.html', 'searchbox.html' ],
   'glossary'             : ['localtoc.html', 'relations.html', 'sourcelink.html', 'searchbox.html' ],
   'change_history'       : ['localtoc.html', 'relations.html', 'sourcelink.html', 'searchbox.html' ]
}



# Additional templates that should be rendered to pages, maps page names to
# template names.
#html_additional_pages = {}

# If false, no module index is generated.
#html_domain_indices = True

# If false, no index is generated.
#html_use_index = True

# If true, the index is split into individual pages for each letter.
#html_split_index = False

# If true, links to the reST sources are added to the pages.
#html_show_sourcelink = True

# If true, "Created using Sphinx" is shown in the HTML footer. Default is True.
#html_show_sphinx = True

# If true, "(C) Copyright ..." is shown in the HTML footer. Default is True.
#html_show_copyright = True

# If true, an OpenSearch description file will be output, and all pages will
# contain a <link> tag referring to it.  The value of this option must be the
# base URL from which the finished HTML is served.
#html_use_opensearch = ''

# This is the file name suffix for HTML files (e.g. ".xhtml").
#html_file_suffix = None

# Output file base name for HTML help builder.
htmlhelp_basename = 'EcflowTutorialdoc'


# -- Options for LaTeX output --------------------------------------------------

# The paper size ('letter' or 'a4').
#latex_paper_size = 'letter'

# The font size ('10pt', '11pt' or '12pt').
#latex_font_size = '10pt'

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title, author, documentclass [howto/manual]).
latex_documents = [
  ('index', 'EcflowTutorial.tex', u'ecFlow',
   u'Avi Bahra', 'manual'),
]

# The name of an image file (relative to this directory) to place at the top of
# the title page.
#latex_logo = None

# For "manual" documents, if this is true, then toplevel headings are parts,
# not chapters.
#latex_use_parts = False

# If true, show page references after internal links.
#latex_show_pagerefs = False

# If true, show URL addresses after external links.
#latex_show_urls = False

# Additional stuff for the LaTeX preamble.
#latex_preamble = ''

# Documents to append as an appendix to all manuals.
#latex_appendices = []

# If false, no module index is generated.
#latex_domain_indices = True


# -- Options for manual page output --------------------------------------------

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    ('index', 'ecflowtutorial', u'ecFlow',
     [u'Avi Bahra'], 1)
]

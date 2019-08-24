# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# http://www.sphinx-doc.org/en/master/config

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.

import os
import sys
sys.path.insert(0, os.path.abspath('../../'))

# -- Project information -----------------------------------------------------

project = 'Elsix'
copyright = '2019, Robert Jacobson'
author = 'Robert Jacobson'

# The full version, including alpha/beta/rc tags
release = '0.1'

# -- Breathe configuration ---------------------------------------------------
# Add the breathe path to your conf.py by adding the following line:
#   sys.path.append( "/home/me/docproj/ext/breathe/" )
# Add breathe as an extension the line could look like this:
#   extensions = ['sphinx.ext.pngmath', 'sphinx.ext.t_odo', 'breathe' ]
# Tell breathe about the projects:
#   breathe_projects = { "myproject": "/home/me/docproj/doxyxml/" }
# Specify a default project:
#   breathe_default_project = "myproject"


# sys.path.append( "/home/me/docproj/ext/breathe/" ) ????
breathe_projects = { "Elsix": os.path.abspath('../../../L6/')}
breathe_default_project = "Elsix"
breathe_implementation_filename_extensions = ['cpp', 'hpp']
breathe_projects_source = {
    "Elsix" : ( "/Users/rjacobson/Development/L6/src",
                ['ast.hpp', 'error.hpp', 'operators.hpp', 'parser.hpp', 'string_utilities.hpp', 'visitor.hpp'] )
}

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = ['sphinx.ext.autodoc', 'sphinx.ext.todo', 'sphinx.ext.mathjax','breathe']

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['examples', 'knowlton1966', 'Manuals', 'build']



# Tell sphinx what the primary language being documented is.
primary_domain = 'cpp'

# Tell sphinx what the pygments highlight language should be.
highlight_language = 'cpp'



# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'alabaster'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

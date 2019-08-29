from distutils.core import setup, Extension

rawDataProcessorModule = Extension('rawDataProcessor',
			 include_dirs = ['/usr/include'],
			 library_dirs = ['/usr/lib'],
			 sources = ['rawDataProcessor.c'])

setup(name = 'rawDataProcessor', version = '1.0', ext_modules = [rawDataProcessorModule])


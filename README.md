# DataStore

## Acknowledgments 

The Slicer Data Store was hosted by Kitware using Midas Platform.

This work was supported by NIH Grant 3P41RR013218-12S1, NA-MIC, NAC, and the Slicer community.

## Contributors

* Jean-Christophe Fillion-Robin
* Charles Marion
* Jean-Baptiste Berger
* Hina Shah
* Pablo Hernandez-Cerdan
* Steve Pieper

## History

In 2013, this Slicer module called "Data Store" was created. It allowed to easily
upload, download and browse [MRB][mrb] files stored in a remote database that was
available at `https://slicer.kitware.com/midas3/folder/1555`. Source code of the module
was stored in the [Slicer/Slicer-DataStore][slicer-datastore-module] GitHub project.

In March 2020, the Slicer community decided (see issue [Slicer/Slicer#4602][slicer-issue-4602])
to retire the DataStore module and instead store the corresponding data files as GitHub release
assets into the [Slicer/SlicerDataStore][slicerdatastore] GitHub project.

[mrb]: https://www.slicer.org/wiki/Documentation/4.10/SlicerApplication/SupportedDataFormat
[slicer-datastore-module]: https://github.com/Slicer/Slicer-DataStore
[slicer-issue-4602]: https://github.com/Slicer/Slicer/issues/4602
[slicerdatastore]: https://github.com/Slicer/SlicerDataStore

## Usage

How to install the Slicer Data Store:

- Compile module using CMakeLists.txt
- Open Slicer
- Edit > Application Settings
- Add an additional module path : path/to/build/lib/Slicer-4.2/qt-loadable-modules
- Restart Slicer

The new module will appear in the all module list as DataStore


## License

This project is distributed under the BSD-style Slicer license allowing academic and commercial use without any restrictions. Please see the [License.txt](License.txt) file for details.

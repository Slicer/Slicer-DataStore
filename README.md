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

In September 2021, the [Slicer legacy extensions catalog][slicer-extensions-legacy-webapp]
has been updated (see issue [Slicer/Slicer#5845][slicer-issue-5845]) to include the
`/midas3/slicerdatastore` and `/midas3/slicerdatastore/user/login` endpoints displaying
instructions referencing the GitHub release assets. Additionally, the "Data Store" module
was improved to set the legacy website theme based on the select Slicer style (`Dark Slicer`
vs `Light Slicer`).

| Light Slicer | Dark Slicer |
|--------------|-------------|
| ![image](https://user-images.githubusercontent.com/219043/133322172-690ab2b6-43d5-472a-a919-dcf96fa84f4a.png) | ![image](https://user-images.githubusercontent.com/219043/133321917-d9fb5412-24d8-42b0-9811-9f141c7c5003.png) |

<!-- Images referenced above are available at https://github.com/Slicer/Slicer/pull/5851#issuecomment-919454598 -->

[mrb]: https://www.slicer.org/wiki/Documentation/4.10/SlicerApplication/SupportedDataFormat
[slicer-datastore-module]: https://github.com/Slicer/Slicer-DataStore
[slicer-issue-4602]: https://github.com/Slicer/Slicer/issues/4602
[slicerdatastore]: https://github.com/Slicer/SlicerDataStore
[slicer-extensions-legacy-webapp]: https://github.com/KitwareMedical/slicer-extensions-legacy-webapp#readme
[slicer-issue-5845]: https://github.com/Slicer/Slicer/issues/5845

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

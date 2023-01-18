# 16 Games in C++

This project (work in progress) was inspired by a series of videos ["Let's make 16 games in C++"](https://www.youtube.com/watch?v=zH_omFPqMO4&list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr). These series of videos present the process of developing 16, simple games written in C++ using [SFML library](https://www.sfml-dev.org/). Author of these videos deliberately has written these games not using the best practices to give his viewers a chance to practice writing these games on their own.

During the development of these games I have partially aid myself with the source code presented in the videos as the aim of this project was to educate myself. The majority of the games were simple to write. The harder to understand games (in terms of development) have their own readme to ease the understanding of the development of their most crucial parts. This harder to understand games are:
- [Minesweeper](05_Minesweeper)
- [Outrun](08_Outrun)
- [Bejeweled](10_Bejeweled)

## Building and Running the games

- Download [SFML (GCC 7.3.0 MinGW (DW2) - 32-bit)](https://www.sfml-dev.org/download.php). SFML should be organized in this project folder, as presented in [tree.txt](tree.txt) (Simply extract and rename appropriately SFML.)
- For building the games MinGW is required. Having installed MinGW run script *Build.cmd*.
- To run the games one should run script *Run.cmd*, which includes the path to necessary DLLs for the game running.

## License

This project is released under the [MIT License](LICENSE).

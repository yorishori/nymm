# NYMM (Not Your Music Manager) <abbr title="Work In Progress">[WIP]</abbr>
Tired of having to enter the terminal, navigaye beets confusing syntax, making sure you are changing the correct files? 
Don't you just want to change Genres to the whole album, or consolidate that annoying artist that names themselves 3 different ways, or simply make sure that the album art is embedded on to all files? 

Don't you want a self-hosted web-app that can give you pretty statistics about your music collection? 

What about an app that doesn't doesn't rely on a propriety library file? An app where the filesystem is the truth.


**NO!?!?** - `(ﾉﾟ0ﾟ)ﾉ~`

---

Yeah, me neither. There are a bunch of tools that do what I intend that will work much better and will be more reliable `(*≧▽≦)`

Anyways, welcome to my hand-made attempt at creating a full-stack application that does this.

> TLDR; a self-hosted web-app that connects to your physical music library and let's you edit your files with a per-Album philosophy. This will be a very opinionated app in terms of style and functionality (that's where the name comes from). 

## Main Features _(in no particular order)_
- Album-first approach.
- Manage tags the easy way and avoid errors/misspellings.
- Library analisis to check for malformatted tags/ missing art/ duplications.
- Add new files, inbox-style; review before adding new items to your main folder.
- Reorgnize file structure of the library.
- Robust verification and confirmation system before actually modifying the files.
- Tag statistics, just for fun`（〜^∇^)〜`
- Files as source of truth, DB is just there to make the page load faster.
- Navidrome integration; because that's what I use to listen to my music.

**IMPORTANT**: This is <mark>NOT</mark> a music player, and it will never become one.

## Underlying Technology
- **Back-end**
    - Main Framework: [Drogon Framework](https://drogon.org/)
    - Storage: [SQLite](https://sqlite.org/)
    - Files: [TagLib](https://taglib.org/)
- **Front-end** _(Served statically through Drogon)_
    - Toolchain: [Vite](https://vite.dev/)
    - UI Framework: [Vue.js](https://vuejs.org/)
- **Languages**: C++, TypeScript

## Getting Started
You will need somethings before you get the project up and running.

### Set-up the environment
Arch packages ~(iusearchbtw)~:
```bash
sudo pacman -S base-devel cmake pkgconf openssl zlib jsoncpp util-linux-libs taglib sqlite
```
To get the Dorogon submodule:
```bash
git submodule update --init --recursive
```
### Executing from project
Made my life easier; just run the `build.sh` and then the `run.sh` command. If you don't have bash (or something compatible), then though luck `(٭°̧̧̧ω°̧̧̧٭)`.
JK. Here are the steps:
1. Build backend with CMake (check out [build.sh](build.sh) for inspiration `☆～（ゝ。∂）`).
1. Build frontend with [WIP].
1. Copy the backend binary, frontend artefacts, and [config.json](src/config.json) to the same folder.
1. Run the binary and behold the wonders of the half-functioning app `ヽ( ★ω★)ノ`


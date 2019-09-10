# re-discord


[![CircleCI](https://circleci.com/gh/yourgithubhandle/re-discord/tree/master.svg?style=svg)](https://circleci.com/gh/yourgithubhandle/re-discord/tree/master)


**Contains the following libraries and executables:**

```
re-discord@0.0.0
│
├─test/
│   name:    TestReDiscord.exe
│   main:    TestReDiscord
│   require: re-discord.lib
│
├─library/
│   library name: re-discord.lib
│   namespace:    ReDiscord
│   require:
│
└─executable/
    name:    ReDiscordApp.exe
    main:    ReDiscordApp
    require: re-discord.lib
```

## Developing:

```
npm install -g esy
git clone <this-repo>
esy install
esy build
```

## Running Binary:

After building the project, you can run the main binary that is produced.

```
esy x ReDiscordApp.exe
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```

# re-discord


[![CircleCI](https://circleci.com/gh/yourgithubhandle/re-discord/tree/master.svg?style=svg)](https://circleci.com/gh/yourgithubhandle/re-discord/tree/master)


**Contains the following libraries and executables:**

```
re-discord@0.0.1
│
├─library/
│   library name: re-discord.lib
│   namespace:    ReDiscord
│   require:
│
└─executable/
    name:    PingBot.exe
    main:    PingBot
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
export DISCORD_BOT_TOKEN={INSERT_TOKEN}
esy x PingBot.exe
```
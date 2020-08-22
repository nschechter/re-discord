# re-discord

## Overview

Re-Discord is a library that provides an API for creating very simple [discord bots](https://discord.com/developers/docs/intro)! This library is catered towards creating bots that serve **up to a few guilds (servers)**, with possible support for sharding in the future.

## Developing:

```
npm install -g esy
git clone <this-repo>
esy install
esy build
```

## Running PingBot Example:

After building the project, you can run the main binary that is produced.

```
export DISCORD_BOT_TOKEN={INSERT_TOKEN}
esy x PingBot.exe
```

## Documentation:

### Supported Events:

```
onReady
onMessage(message)
onGuildMemberAdd(guild, member)
onGuildMemberRemove(guild, member)
onReactionAdd(message)
onReactionRemove(message)
```

### Sending a message

```
Channel.send = (content: string, channel: Channel.t)
Message.reply = (content: string, message: Message.t)
Message.react = (emoji: string, message: Message.t)
```

### Future Plans

```
- Resuming
- Voice Support
```
# re-discord


**Contains the following libraries and executables:**

```
re-discord@1.1.0
│
├─library/
│   library name: re-discord.lib
│   namespace:    ReDiscord
│
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
- Attachments
- Higher level API
- More events!
- Voice Support
- Sharding
```
type http = {
  baseApi: string,
  version: string,
};

type endpoints = {botGateway: string};

let http = {baseApi: "https://discordapp.com/api", version: "7"};

let endpoints = {botGateway: "/gateway/bot"};

module DiscordOpCodes = {
  type t =
    | DISPATCH
    | HEARTBEAT
    | IDENTIFY
    | STATUS_UPDATE
    | VOICE_STATE_UPDATE
    | VOICE_GUILD_PING
    | RESUME
    | RECONNECT
    | REQUEST_GUILD_MEMBERS
    | INVALID_SESSION
    | HELLO
    | HEARTBEAT_ACK;

  let to_enum =
    fun
    | DISPATCH => 0
    | HEARTBEAT => 1
    | IDENTIFY => 2
    | STATUS_UPDATE => 3
    | VOICE_STATE_UPDATE => 4
    | VOICE_GUILD_PING => 5
    | RESUME => 6
    | RECONNECT => 7
    | REQUEST_GUILD_MEMBERS => 8
    | INVALID_SESSION => 9
    | HELLO => 10
    | HEARTBEAT_ACK => 11;
};
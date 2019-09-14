type author = {
  username: string,
  id: string,
  discriminator: string,
  avatar: string,
};

type t = {
  token: string,
  id: string,
  guildId: string,
  content: string,
  channelId: string,
  author,
  timestamp: string,
};

let make = (token, id, guildId, content, channelId, author, timestamp): t => {
  token,
  id,
  guildId,
  content,
  channelId,
  author,
  timestamp,
};

let reply = (content: string, message: t) => {
  Channel.createMessage(message.token, message.channelId, content) |> ignore;
  message;
};

let react = (emoji: string, message: t) => {
  Channel.createReact(message.token, message.channelId, message.id, emoji)
  |> ignore;
  message;
};
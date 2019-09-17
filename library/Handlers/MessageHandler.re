let handle = (token, data, onMessage) => {
  data |> Message.extract(token) |> onMessage;
};

let handleReaction = (token, data, handler) => {
  data |> MessageReaction.extract(token) |> handler;
};
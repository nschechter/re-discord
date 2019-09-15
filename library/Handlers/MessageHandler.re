let handle = (data, onMessage) => {
  data |> Message.extract |> onMessage;
};

let handleReaction = (data, handler) => {
  data |> MessageReaction.extract |> handler;
};
let handle = (token, data, onMessage) => {
  data |> Message.extract(token) |> onMessage;
};

let handleReaction = data => {
  data |> MessageReaction.extract;
};
let handle = (token, data, onMessage) => {
  data |> Message.extract(token) |> onMessage;
};
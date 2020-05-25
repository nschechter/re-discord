let handle = (_, payload, handler) => {
  switch (handler) {
  | Some(h) => h()
  | None => ignore()
  };

  Ready.extract(payload);
};
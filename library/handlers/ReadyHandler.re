let handle = (state, payload, handler) => {
  switch (handler) {
  | Some(h) => h()
  | None => ignore()
  };

  Ready.extract(payload);
};
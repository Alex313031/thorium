'''Custom error types.
'''


class UserInstructions(Exception):
  '''Handy exception subclass that just prints very verbose instructions to the
  user. Normal exceptions tend to lose the message in the stack trace, which we
  probably don't care about.
  '''
  __slots__ = ('_msg', )
  def __init__ (self, msg):
    self._msg = msg

  def __str__(self):
    sep = '=' * 78
    return f'\n\n{sep}\n{self._msg}\n{sep}\n\n'
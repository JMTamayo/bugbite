import logging

from rich.console import Console
from rich.logging import RichHandler
from rich.theme import Theme

_THEME = Theme(
    {
        "logging.level.debug": "bold cyan",
        "logging.level.info": "bold green",
        "logging.level.warning": "bold yellow",
        "logging.level.error": "bold red",
        "logging.level.critical": "bold white on red",
    }
)

_console = Console(theme=_THEME)


def get_logger(name: str) -> logging.Logger:
    """Return a logger with colored Rich output.

    Args:
        name: Logger name, typically the module's ``__name__``.

    Returns:
        Configured :class:`logging.Logger` instance.
    """
    logger = logging.getLogger(name)

    if logger.handlers:
        return logger

    handler = RichHandler(
        console=_console,
        show_time=True,
        show_level=True,
        show_path=True,
        rich_tracebacks=True,
        markup=True,
    )

    logger.setLevel(logging.DEBUG)
    logger.addHandler(handler)
    logger.propagate = False

    return logger

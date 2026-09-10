"""Abstract context base — shared path resolution contract."""

from __future__ import annotations

from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class ContextBase(ABC):
    """Abstract base for all pipeline contexts.

    Subclasses define domain-specific path resolution via properties.
    The pipeline orchestrator uses only the abstract properties defined here.
    """
    slug: str
    root_dir: Path
    mode: str = "standard"
    skip_stages: set[str] = field(default_factory=set)
    verbose: bool = False

    @property
    @abstractmethod
    def contract_path(self) -> Path:
        """Path to the project/family contract file (project.json / contract.json)."""
        ...

    @property
    @abstractmethod
    def entry_exe_path(self) -> Path:
        """Path to the built native entry executable (entry.exe / entry-aot.exe)."""
        ...

    @property
    @abstractmethod
    def results_dir(self) -> Path:
        """Output directory for verification results and reports."""
        ...

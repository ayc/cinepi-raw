# Product Guidelines

## Communication Tone
- **Technical and Precise:** All documentation, logs, and user-facing communications must prioritize technical accuracy. Use professional terminology and provide detailed specifications to ensure clarity for developers and advanced users.

## Visual Identity & UX
- **Industrial and Functional:** Interfaces (both CLI and future GUI) should prioritize high-contrast data readouts and a "form follows function" layout. The goal is to provide immediate, unambiguous information during technical camera operations.

## Core Quality Principles
- **Predictability:** The application must behave consistently. Error messages and logs should be descriptive and actionable, facilitating rapid troubleshooting in the field.
- **Maintainability:** Code changes should favor patterns that simplify rebasing against upstream repositories. Avoid unnecessary complexity that deviates from established `libcamera-apps` conventions.

## Maintenance and Contribution Strategy
- **Upstream-First:** We prioritize contributing improvements to core libraries back to the official Raspberry Pi repositories whenever possible. This "thin fork" strategy minimizes our long-term maintenance burden and ensures CinePi Raw benefits from the latest official stability updates.

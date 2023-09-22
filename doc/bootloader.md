## Bootloader

```mermaid
stateDiagram
    [*] --> PAGE_START
    PAGE_START --> PAGE_FILL_NEXT
    PAGE_FILL_NEXT --> PAGE_FILL
    PAGE_FILL --> PAGE_FILL_NEXT
    PAGE_FILL --> PAGE_WRITED
    PAGE_WRITED --> PAGE_START
    PAGE_WRITED --> [*]
```

# Spada SpGEMM + Trace Generator + Mapper

This repo contains all the files required to:

✅ Multiply sparse matrices using SPADA's Window-Based Adaptive Dataflow  
✅ Log every memory access during matrix multiplication  
✅ Extract A-row-based memory access mappings for RidxLRU

---

## 🧩 Folder Structure

| File               | Description                                 |
|--------------------|---------------------------------------------|
| `spada_wadf_final.c` | WA-based SpGEMM implementation (Algorithm 1) |
| `trace_logger.c`     | Logs trace.txt + a_row_index.txt           |
| `extract_mapper.c`   | Creates addr_to_arow.map for RidxLRU       |
| `trace.txt`          | r/w memory accesses                        |
| `a_row_index.txt`    | Which A-row caused each access             |
| `addr_to_arow.map`   | Output map: addr → a_row                   |

---

## 🛠 Build and Run

```bash
make                      # builds all
./trace_logger            # generates trace.txt + a_row_index.txt
./extract_mapper          # generates addr_to_arow.map
./spada                   # runs WA SpGEMM (adjust α, β inside code)

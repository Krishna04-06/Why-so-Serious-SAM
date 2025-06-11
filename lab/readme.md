# Why-so-Serious-SAM: Fileless Ransomware Red Team Simulation Lab

## 🎯 Objective

Design and deploy a **realistic fileless ransomware simulation** that mirrors modern threat actor tradecraft, leveraging:

* MITRE ATT\&CK-aligned techniques
* LOLBins (Living Off the Land Binaries)
* Reflective DLL injection (e.g., `nsfw.dll`)
* DiskCryptor-based AES-XTS disk impact
* Blue Team detection with ELK Stack + Sigma

The end goal is to showcase **advanced offensive and defensive skills** in a professional cybersecurity setting.

---

## 🧪 Lab Architecture

### Red Team

* **Initial Access**: Print Spooler exploit (T1210)
* **Execution**: Fileless DLL via `rundll32.exe` (T1218.011)
* **Persistence**: Scheduled tasks via `schtasks.exe` (T1053.005)
* **Lateral Movement**: PsExec/WMIC with valid credentials (T1078.003, T1021.002)
* **Credential Access**: LSASS dump (T1003.001)
* **Defense Evasion**: Event log clearing (T1070.001)
* **Impact**: Data encryption and system reboot (T1486, T1529)

### Blue Team

* **Sensors**:

  * Sysmon (Windows Event Logging)
  * Winlogbeat (Log forwarder)
  * Elastic Agent (optional)

* **SIEM**: ELK Stack (ElasticSearch, Logstash, Kibana)

* **Detection**:

  * Sigma rules (mapped to TTPs)
  * LOLBin execution monitoring
  * High entropy file/disk detection

---

## 💡 Integration with LOLBins

Use LOLBins from [lolol.farm](https://lolol.farm/) to avoid AV/EDR detection:

| Technique | LOLBins Used                                  |
| --------- | --------------------------------------------- |
| T1486     | `cipher.exe`, `bcdedit.exe`, `manage-bde.exe` |
| T1210     | `rundll32.exe`, `reg.exe`, `spoolsv.exe`      |
| T1036     | `PsExec.exe` renamed to `dllhost.dat`         |
| T1053.005 | `schtasks.exe`, `powershell.exe`              |
| T1218.011 | `rundll32.exe`                                |

---

## 📊 ELK Detection Dashboard Panels

### 🔍 Panels to Create

* **LOLBin Executions**: `process.name: ("rundll32.exe" OR "wevtutil.exe" OR "PsExec.exe")`
* **High Entropy File Activity**: Identify ransomware-like behavior
* **Scheduled Tasks**: Visualize new task creations
* **Log Tampering**: Detect `wevtutil.exe` clearing
* **Credential Dumps**: `procdump.exe`, `mimikatz` detections

### 🎯 MITRE Mapping Panel

* Integrate with Elastic’s ATT\&CK navigator or Kibana Canvas
* Use custom tags to align logs with MITRE TTPs

---

## 📑 Documentation Template for Showcase (Interview)

### 1. Executive Summary

* Short 1-pager on simulation purpose and key takeaways

### 2. Red Team Report

* Step-by-step execution plan
* LOLBins used and mapped ATT\&CK techniques
* Sample encoded command or shellcode

### 3. Blue Team Report

* Screenshots of ELK alerts, panels
* Detected Sigma matches
* Detection gaps and how they were closed

### 4. Lessons Learned

* Strengths of simulation
* How it mimics real-world APT techniques
* What can be improved (e.g., DCRAT-style remote ops, DGA usage, etc.)

---

## ✅ Professional Tips for Interview Showcase

1. **Portfolio Format**: Host on GitHub Pages or Notion with visuals and links to Sigma/ELK configs.
2. **Readme Quality**: Write a crystal-clear `README.md` that explains:

   * Objectives
   * Techniques used (with MITRE ID)
   * Tools used and why
   * Screenshots or diagrams
3. **Demo Video** (optional): 3-5 minute narrated walkthrough (OBS Studio).
4. **CI/CD Touch**: Use GitHub Actions or simple scripts to auto-push config updates.
5. **Blue Team Insight**: Mention what you *couldn’t* detect and how you'd tune next time.
6. **Map to Real Threat Actors**: E.g., align to FIN12, Conti, Wizard Spider with ATT\&CK mapping.

---

## 🔚 Outcome

This project provides:

* Demonstrable **red and blue team capability**
* Ability to script, inject, detect, and analyze advanced malware
* Communication of technical findings in a structured, actionable format

A strong asset for any offensive or defensive security role.

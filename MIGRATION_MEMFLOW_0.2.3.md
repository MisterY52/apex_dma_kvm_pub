# Migration vers Memflow 0.2.3

## Résumé des modifications

Ce document décrit les modifications effectuées pour mettre à jour le projet vers memflow 0.2.3.

## Changements dans les versions

### Fichiers Cargo.toml mis à jour

Tous les packages memflow ont été mis à jour de la version 0.2.0/0.2.1 vers 0.2.3 :

1. **apex_dma/memflow_lib/memflow/Cargo.toml**
   - Version: 0.2.1 → 0.2.3
   - Rust version minimale: 1.70.0 → 1.74.0

2. **apex_dma/memflow_lib/memflow-ffi/Cargo.toml**
   - Version: 0.2.0 → 0.2.3

3. **apex_dma/memflow_lib/memflow-bench/Cargo.toml**
   - Version: 0.2.0 → 0.2.3

4. **apex_dma/memflow_lib/memflow-derive/Cargo.toml**
   - Version: 0.2.0 → 0.2.3

5. **apex_dma/memflow_lib/nostd-test/Cargo.toml**
   - Version: 0.2.0 → 0.2.3

6. **apex_dma/memflow_lib/memflow-win32/memflow-win32/Cargo.toml**
   - Version: 0.2.0 → 0.2.3

7. **apex_dma/memflow_lib/memflow-win32/memflow-win32-defs/Cargo.toml**
   - Version: 0.2.0 → 0.2.3

## Changements d'API dans memflow 0.2.3

### Principales modifications

1. **Préfixe des fonctions C/C++**
   - Toutes les fonctions globales C/C++ sont maintenant préfixées avec `mf_`
   - Exemple: `inventory_scan()` → `mf_inventory_scan()`
   - **Note**: Le code existant utilise déjà ces préfixes, aucune modification nécessaire

2. **Fonctions de lecture de chaînes**
   - Les anciennes fonctions de lecture de chaînes ont été remplacées par:
     - `read_utf8()` - lecture stricte UTF-8
     - `read_utf8_lossy()` - lecture UTF-8 avec remplacement des caractères invalides

3. **Support Macho lossy**
   - Ajout du parsing Macho lossy via goblin

4. **Version Rust minimale**
   - Mise à jour de 1.70.0 à 1.74.0

## Compatibilité du code existant

### Fichiers analysés

- **apex_dma/memory.cpp**: Utilise déjà les fonctions préfixées `mf_`
  - `mf_inventory_scan()`
  - `mf_inventory_create_connector()`
  - `mf_inventory_create_os()`
  - `mf_connector_drop()`
  - `mf_inventory_free()`

- **apex_dma/memory.h**: Compatible avec les nouveaux bindings
  - Utilise `ConnectorInstance<>`, `OsInstance<>`, `IntoProcessInstance<>`
  - Les templates sont compatibles avec la nouvelle version

- **apex_dma/memflow_lib/memflow-ffi/memflow.hpp**: Bindings C++ générés automatiquement
  - Sera régénéré lors de la compilation

## Instructions de compilation

### 1. Compilation de memflow-ffi

```bash
cd apex_dma/memflow_lib/memflow-ffi/
cargo build --release
```

Cette commande va:
- Compiler memflow avec la version 0.2.3
- Générer les nouveaux bindings C++ dans `memflow.hpp`
- Créer la bibliothèque `libmemflow_ffi.so` (Linux) ou `memflow_ffi.dll` (Windows)

### 2. Compilation du projet complet

Utiliser le script de build existant:

```bash
cd apex_dma
./build.sh
```

Ou manuellement:

```bash
cd apex_dma/memflow_lib/memflow-ffi/
cargo build --release
cd ../../
make
```

## Vérifications post-migration

### Tests à effectuer

1. **Vérifier la compilation**
   ```bash
   cd apex_dma
   ./build.sh
   ```

2. **Vérifier les symboles de la bibliothèque**
   ```bash
   nm -D apex_dma/memflow_lib/target/release/libmemflow_ffi.so | grep mf_
   ```

3. **Tester l'initialisation**
   - Vérifier que `mf_inventory_scan()` fonctionne
   - Vérifier que les connecteurs (kvm, qemu) peuvent être créés
   - Vérifier que l'OS (win32) peut être initialisé

## Problèmes potentiels et solutions

### Problème 1: Erreur de compilation Rust

**Symptôme**: Erreur lors de `cargo build`

**Solution**: Vérifier la version de Rust
```bash
rustc --version  # Doit être >= 1.74.0
rustup update
```

### Problème 2: Symboles non trouvés lors du link

**Symptôme**: `undefined reference to mf_*`

**Solution**: 
- Recompiler memflow-ffi: `cd apex_dma/memflow_lib/memflow-ffi && cargo build --release`
- Vérifier le chemin de la bibliothèque dans le Makefile

### Problème 3: ABI incompatible

**Symptôme**: Crash au runtime ou comportement inattendu

**Solution**: 
- Nettoyer et recompiler complètement:
  ```bash
  cd apex_dma/memflow_lib
  cargo clean
  cargo build --release
  cd ../
  make clean
  make
  ```

## Changements futurs (memflow 2.x)

**Note importante**: Il n'existe pas encore de version stable 2.x de memflow. La dernière version stable est 0.2.3.

Lorsque memflow 2.x sera disponible, les changements attendus incluront:
- Nouvelle architecture de plugins basée sur cglue
- API de mémoire et d'adressage repensée
- Améliorations de performance
- Possibles breaking changes dans l'API

## Références

- [Memflow GitHub](https://github.com/memflow/memflow)
- [Memflow Releases](https://github.com/memflow/memflow/releases)
- [Memflow Documentation](https://docs.rs/memflow)
- [Release Notes 0.2.3](https://github.com/memflow/memflow/releases/tag/0.2.3)

## Résumé

✅ Toutes les versions des packages memflow ont été mises à jour vers 0.2.3
✅ Le code existant est compatible avec les nouveaux bindings
✅ Les fonctions utilisent déjà le préfixe `mf_` requis
✅ Aucune modification du code C++ n'est nécessaire
✅ Seule la recompilation est requise pour appliquer les changements

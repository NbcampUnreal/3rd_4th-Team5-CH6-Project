# TinySurvivor

#### Object Pooling

- Efficient Actor Pool Plugin은 PIE 종료 시 GC 크래시 버그 있음
    - 이 버그 해결을 위해 ActorPool.h의 AllSpawnedActors를 private에서 public으로 강제 수정
    - 플러그인 업데이트 시, 이 수정을 반드시 다시 적용해야함
/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: address_book.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "address_book.pb-c.h"
void   tutorial__person__phone_number__init
                     (Tutorial__Person__PhoneNumber         *message)
{
  static const Tutorial__Person__PhoneNumber init_value = TUTORIAL__PERSON__PHONE_NUMBER__INIT;
  *message = init_value;
}
void   tutorial__person__init
                     (Tutorial__Person         *message)
{
  static const Tutorial__Person init_value = TUTORIAL__PERSON__INIT;
  *message = init_value;
}
size_t tutorial__person__get_packed_size
                     (const Tutorial__Person *message)
{
  assert(message->base.descriptor == &tutorial__person__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t tutorial__person__pack
                     (const Tutorial__Person *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &tutorial__person__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t tutorial__person__pack_to_buffer
                     (const Tutorial__Person *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &tutorial__person__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Tutorial__Person *
       tutorial__person__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Tutorial__Person *)
     protobuf_c_message_unpack (&tutorial__person__descriptor,
                                allocator, len, data);
}
void   tutorial__person__free_unpacked
                     (Tutorial__Person *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &tutorial__person__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   tutorial__addressbook__init
                     (Tutorial__Addressbook         *message)
{
  static const Tutorial__Addressbook init_value = TUTORIAL__ADDRESSBOOK__INIT;
  *message = init_value;
}
size_t tutorial__addressbook__get_packed_size
                     (const Tutorial__Addressbook *message)
{
  assert(message->base.descriptor == &tutorial__addressbook__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t tutorial__addressbook__pack
                     (const Tutorial__Addressbook *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &tutorial__addressbook__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t tutorial__addressbook__pack_to_buffer
                     (const Tutorial__Addressbook *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &tutorial__addressbook__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Tutorial__Addressbook *
       tutorial__addressbook__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Tutorial__Addressbook *)
     protobuf_c_message_unpack (&tutorial__addressbook__descriptor,
                                allocator, len, data);
}
void   tutorial__addressbook__free_unpacked
                     (Tutorial__Addressbook *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &tutorial__addressbook__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const Tutorial__Person__PhoneType tutorial__person__phone_number__type__default_value = TUTORIAL__PERSON__PHONE_TYPE__HOME;
static const ProtobufCFieldDescriptor tutorial__person__phone_number__field_descriptors[2] =
{
  {
    "number",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Tutorial__Person__PhoneNumber, number),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "type",
    2,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_ENUM,
    offsetof(Tutorial__Person__PhoneNumber, has_type),
    offsetof(Tutorial__Person__PhoneNumber, type),
    &tutorial__person__phone_type__descriptor,
    &tutorial__person__phone_number__type__default_value,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned tutorial__person__phone_number__field_indices_by_name[] = {
  0,   /* field[0] = number */
  1,   /* field[1] = type */
};
static const ProtobufCIntRange tutorial__person__phone_number__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor tutorial__person__phone_number__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "tutorial.Person.PhoneNumber",
  "PhoneNumber",
  "Tutorial__Person__PhoneNumber",
  "tutorial",
  sizeof(Tutorial__Person__PhoneNumber),
  2,
  tutorial__person__phone_number__field_descriptors,
  tutorial__person__phone_number__field_indices_by_name,
  1,  tutorial__person__phone_number__number_ranges,
  (ProtobufCMessageInit) tutorial__person__phone_number__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCEnumValue tutorial__person__phone_type__enum_values_by_number[3] =
{
  { "MOBILE", "TUTORIAL__PERSON__PHONE_TYPE__MOBILE", 0 },
  { "HOME", "TUTORIAL__PERSON__PHONE_TYPE__HOME", 1 },
  { "WORK", "TUTORIAL__PERSON__PHONE_TYPE__WORK", 2 },
};
static const ProtobufCIntRange tutorial__person__phone_type__value_ranges[] = {
{0, 0},{0, 3}
};
static const ProtobufCEnumValueIndex tutorial__person__phone_type__enum_values_by_name[3] =
{
  { "HOME", 1 },
  { "MOBILE", 0 },
  { "WORK", 2 },
};
const ProtobufCEnumDescriptor tutorial__person__phone_type__descriptor =
{
  PROTOBUF_C__ENUM_DESCRIPTOR_MAGIC,
  "tutorial.Person.PhoneType",
  "PhoneType",
  "Tutorial__Person__PhoneType",
  "tutorial",
  3,
  tutorial__person__phone_type__enum_values_by_number,
  3,
  tutorial__person__phone_type__enum_values_by_name,
  1,
  tutorial__person__phone_type__value_ranges,
  NULL,NULL,NULL,NULL   /* reserved[1234] */
};
static const ProtobufCFieldDescriptor tutorial__person__field_descriptors[4] =
{
  {
    "name",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Tutorial__Person, name),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "id",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(Tutorial__Person, id),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "email",
    3,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Tutorial__Person, email),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "phones",
    4,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(Tutorial__Person, n_phones),
    offsetof(Tutorial__Person, phones),
    &tutorial__person__phone_number__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned tutorial__person__field_indices_by_name[] = {
  2,   /* field[2] = email */
  1,   /* field[1] = id */
  0,   /* field[0] = name */
  3,   /* field[3] = phones */
};
static const ProtobufCIntRange tutorial__person__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 4 }
};
const ProtobufCMessageDescriptor tutorial__person__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "tutorial.Person",
  "Person",
  "Tutorial__Person",
  "tutorial",
  sizeof(Tutorial__Person),
  4,
  tutorial__person__field_descriptors,
  tutorial__person__field_indices_by_name,
  1,  tutorial__person__number_ranges,
  (ProtobufCMessageInit) tutorial__person__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor tutorial__addressbook__field_descriptors[1] =
{
  {
    "people",
    1,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(Tutorial__Addressbook, n_people),
    offsetof(Tutorial__Addressbook, people),
    &tutorial__person__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned tutorial__addressbook__field_indices_by_name[] = {
  0,   /* field[0] = people */
};
static const ProtobufCIntRange tutorial__addressbook__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor tutorial__addressbook__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "tutorial.Addressbook",
  "Addressbook",
  "Tutorial__Addressbook",
  "tutorial",
  sizeof(Tutorial__Addressbook),
  1,
  tutorial__addressbook__field_descriptors,
  tutorial__addressbook__field_indices_by_name,
  1,  tutorial__addressbook__number_ranges,
  (ProtobufCMessageInit) tutorial__addressbook__init,
  NULL,NULL,NULL    /* reserved[123] */
};
